#ifndef AWTRIX_DISABLE_TIMER
#include "TimerSettings.h"

#include <stdlib.h>
#include <stdio.h>          // snprintf (timerFormatHMS, the bar-color formatters)

// The descriptor-table family is the PURE Timer validation surface: it links
// against ArduinoJson + the enum codec tables ONLY -- no Globals.h (FastLED), no
// TimerManager singleton, no MQTTManager, no Preferences. That dependency-light link
// is what lets TimerCommand::classify be exercised in isolation. The impure
// half -- the member-config apply/emit/publish hooks, the NVS round-trip, and the
// full-config dump, all of which DO touch the singleton / MQTT / Preferences --
// lives in TimerSettingsApply.cpp. The two share this header.
//
// The 13 persisted timer-setting globals are DEFINED here (moved out of Globals.cpp;
// Globals.h keeps the extern decls so every other caller is source-unchanged) so the
// TIMER_SETTINGS_DESCS storage pointers resolve without dragging in Globals.cpp.
uint32_t TIMER_MAX_DURATION      = 86400;
uint16_t TIMER_PUBLISH_INTERVAL  = 1;
uint16_t TIMER_FINISHED_HOLD     = 10;
uint16_t TIMER_REALERT_INTERVAL  = 15;
uint16_t TIMER_COUNTDOWN_SECONDS = 3;
String   TIMER_MELODY_TICK       = "timer_tick";
String   TIMER_MELODY_END        = "timer_end";
bool     TIMER_BAR_ENABLED       = true;
bool     TIMER_ICON_ENABLED      = true;
uint32_t TIMER_BAR_COLOR         = 0;
uint32_t TIMER_BAR_BG_COLOR      = 0;     // 0 = black = no track (literal off)
bool     TIMER_SYNC_FOLLOW       = true;  // fresh clock is a follower; standalone is opt-out
String   TIMER_SYNC_TARGETS      = "";

namespace
{
    // sync_targets accepts "" (off), "all", or a comma list of device-id tokens
    // ([A-Za-z0-9_-], 1..32 each). Moved here from TimerManager.cpp so the table is
    // the single home of the sync_targets validation. Same rule as before.
    bool isValidSyncTargets(const String &s)
    {
        String t = s; t.trim();
        if (t.length() == 0 || t == "all") return true;
        int start = 0;
        const int n = t.length();
        while (start <= n)
        {
            int comma = t.indexOf(',', start);
            if (comma < 0) comma = n;
            String tok = t.substring(start, comma); tok.trim();
            if (tok.length() == 0 || tok.length() > 32) return false;
            for (size_t i = 0; i < tok.length(); ++i)
            {
                char c = tok[i];
                bool ok = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
                          (c >= 'A' && c <= 'Z') || c == '_' || c == '-';
                if (!ok) return false;
            }
            if (comma == n) break;
            start = comma + 1;
        }
        return true;
    }

    // bar_color: a JSON number 0..0xFFFFFF, or an "#RRGGBB" / "RRGGBB" hex string.
    bool parseBarColor(JsonVariantConst v, TcValue &out)
    {
        if (v.is<long>() || v.is<float>())
        {
            uint32_t n = v.as<uint32_t>();
            if (n > 0xFFFFFFu) return false;
            out.num = n;
            return true;
        }
        if (v.is<const char *>() || v.is<String>())
        {
            String s = v.as<String>();
            s.trim();
            if (s.length() > 0 && s[0] == '#') s = s.substring(1);
            if (s.length() != 6) return false;   // exactly RRGGBB
            for (size_t i = 0; i < s.length(); ++i)
            {
                char c = s[i];
                bool ok = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
                if (!ok) return false;
            }
            out.num = (uint32_t)strtoul(s.c_str(), nullptr, 16);
            return true;
        }
        return false;
    }

    // max_duration carrier-native HA-attribute formatter:
    // renders the stored cap (raw seconds, a U32) as the trimmed H:MM:SS clock
    // string the Duration text entity's OWN state speaks, by reusing the exact
    // formatHMS the Duration state uses ("24:00:00", "1:00:00", "0:45"). This is
    // the first key whose attribute representation differs PER CARRIER: the state
    // sensor keeps the raw-seconds number (no formatter), while the Duration
    // carrier renders this clock string — each carrier in its native form, over
    // the same persisted storage, so the underlying value cannot drift.
    void formatMaxDurationHMS(const TimerSettingDesc &d, JsonDocument &doc)
    {
        uint32_t v = *static_cast<uint32_t *>(d.storage);
        doc[d.cmdKey] = timerFormatHMS(v);   // family-local pure formatter
    }

    // sync_targets: strict string type, then the comma-list rule above.
    bool parseSyncTargets(JsonVariantConst v, TcValue &out)
    {
        if (!(v.is<const char *>() || v.is<String>())) return false;
        String s = v.as<String>();
        if (!isValidSyncTargets(s)) return false;
        out.str = s;
        return true;
    }
}

// bar_color / bar_bg_color HA-attribute formatters.
// External linkage (not file-local) so both the attribute-group table here and the
// HTTP full-config dump in TimerSettingsApply.cpp render the stored 0xRRGGBB int the
// same way -- they cannot disagree about the string form. bar_color's off
// sentinel (0) is "default" (follow text color); bar_bg_color's off is
// "none" (black = no track, literal off); any other value is uppercase "#RRGGBB".
void timerFormatBarColor(const TimerSettingDesc &d, JsonDocument &doc)
{
    uint32_t v = *static_cast<uint32_t *>(d.storage);
    if (v == 0) { doc[d.cmdKey] = "default"; return; }
    char buf[8];
    snprintf(buf, sizeof(buf), "#%06X", (unsigned)(v & 0xFFFFFFu));
    doc[d.cmdKey] = buf;
}

void timerFormatBarBgColor(const TimerSettingDesc &d, JsonDocument &doc)
{
    uint32_t v = *static_cast<uint32_t *>(d.storage);
    if (v == 0) { doc[d.cmdKey] = "none"; return; }
    char buf[8];
    snprintf(buf, sizeof(buf), "#%06X", (unsigned)(v & 0xFFFFFFu));
    doc[d.cmdKey] = buf;
}

// One row per persisted value-config Timer key. The two inSnapshot=false rows are
// the sync roles (local identity), excluded from the propagated config block.
//   cmdKey, devKey, nvsKey, type, check, lo, hi, dfltNum, dfltStr, bespoke, inSnapshot, storage
const TimerSettingDesc TIMER_SETTINGS_DESCS[] = {
    {"finished_hold",              "timer_finished_hold",              "TFHOLD",  TcType::U16,  TcCheck::UIntRange, 1,   300,    10,    nullptr,       nullptr,         true,  &TIMER_FINISHED_HOLD},
    {"realert_interval",           "timer_realert_interval",           "TRALERT", TcType::U16,  TcCheck::UIntRange, 5,   300,    15,    nullptr,       nullptr,         true,  &TIMER_REALERT_INTERVAL},
    {"countdown_seconds",          "timer_countdown_seconds",          "TCDOWN",  TcType::U16,  TcCheck::UIntRange, 0,   30,     3,     nullptr,       nullptr,         true,  &TIMER_COUNTDOWN_SECONDS},
    {"max_duration",               "timer_max_duration",               "TMAXD",   TcType::U32,  TcCheck::UIntRange, 1,   604800, 86400, nullptr,       nullptr,         true,  &TIMER_MAX_DURATION},
    {"remaining_publish_interval", "timer_remaining_publish_interval", "TPUBI",   TcType::U16,  TcCheck::UIntRange, 1,   60,     1,     nullptr,       nullptr,         true,  &TIMER_PUBLISH_INTERVAL},
    {"icon_enabled",               "timer_icon_enabled",               "TICONEN", TcType::Bool, TcCheck::Bool,     0,   0,      1,     nullptr,       nullptr,         true,  &TIMER_ICON_ENABLED},
    {"bar_enabled",                "timer_bar_enabled",                "TBAREN",  TcType::Bool, TcCheck::Bool,     0,   0,      1,     nullptr,       nullptr,         true,  &TIMER_BAR_ENABLED},
    {"bar_color",                  "timer_bar_color",                  "TBARC",   TcType::U32,  TcCheck::Bespoke,  0,   0,      0,     nullptr,       parseBarColor,   true,  &TIMER_BAR_COLOR},
    {"bar_bg_color",               "timer_bar_bg_color",               "TBARBC",  TcType::U32,  TcCheck::Bespoke,  0,   0,      0,     nullptr,       parseBarColor,   true,  &TIMER_BAR_BG_COLOR},
    {"melody_tick",                "timer_melody_tick",                "TMTICK",  TcType::Str,  TcCheck::Name,     0,   0,      0,     "timer_tick",  nullptr,         true,  &TIMER_MELODY_TICK},
    {"melody_end",                 "timer_melody_end",                 "TMEND",   TcType::Str,  TcCheck::Name,     0,   0,      0,     "timer_end",   nullptr,         true,  &TIMER_MELODY_END},
    {"sync_follow",                "timer_sync_follow",                "TSYNF",   TcType::Bool, TcCheck::Bool,     0,   0,      1,     nullptr,       nullptr,         false, &TIMER_SYNC_FOLLOW},
    {"sync_targets",               "timer_sync_targets",               "TSYNT",   TcType::Str,  TcCheck::Bespoke,  0,   0,      0,     "",            parseSyncTargets, false, &TIMER_SYNC_TARGETS},
};

const size_t TIMER_SETTINGS_DESC_COUNT = sizeof(TIMER_SETTINGS_DESCS) / sizeof(TIMER_SETTINGS_DESCS[0]);

// The header's compile-time extent (used to size the one-shot snapshot buffer) must
// match the actual table. If a row is added, bump TIMER_SETTINGS_DESC_CAP.
static_assert(sizeof(TIMER_SETTINGS_DESCS) / sizeof(TIMER_SETTINGS_DESCS[0]) == TIMER_SETTINGS_DESC_CAP,
              "TIMER_SETTINGS_DESC_CAP must equal the descriptor row count");

bool timerSettingParse(const TimerSettingDesc &d, JsonVariantConst v, TcValue &out)
{
    if (d.bespoke) return d.bespoke(v, out);

    switch (d.check)
    {
        case TcCheck::Bool:
            if (!v.is<bool>()) return false;
            out.b = v.as<bool>();
            return true;

        case TcCheck::UIntRange:
        {
            if (!(v.is<long>() || v.is<float>())) return false;   // number only (not bool/string/null)
            uint32_t n = v.as<uint32_t>();
            if (n < d.lo || n > d.hi) return false;
            out.num = n;
            return true;
        }

        case TcCheck::Name:
        {
            // Bare filename; same char-rule as icons. Empty resets to the default at
            // coerce time (matches legacy melody semantics). Mirrors the legacy path,
            // which coerced via as<String>() without a strict pre-type-check.
            String s = v.as<String>();
            if (!timerIsValidIconName(s)) return false;
            out.str = (s.length() == 0) ? String(d.dfltStr) : s;
            return true;
        }

        case TcCheck::Bespoke:
            return false;   // unreachable: bespoke rows carry d.bespoke, handled above
    }
    return false;
}

bool timerSettingStore(const TimerSettingDesc &d, const TcValue &v)
{
    switch (d.type)
    {
        case TcType::U16:
        {
            uint16_t *p = static_cast<uint16_t *>(d.storage);
            if (*p == (uint16_t)v.num) return false;
            *p = (uint16_t)v.num;
            return true;
        }
        case TcType::U32:
        {
            uint32_t *p = static_cast<uint32_t *>(d.storage);
            if (*p == v.num) return false;
            *p = v.num;
            return true;
        }
        case TcType::Bool:
        {
            bool *p = static_cast<bool *>(d.storage);
            if (*p == v.b) return false;
            *p = v.b;
            return true;
        }
        case TcType::Str:
        {
            String *p = static_cast<String *>(d.storage);
            if (*p == v.str) return false;
            *p = v.str;
            return true;
        }
    }
    return false;
}

// timerSettingsLoadNvs / timerSettingsSaveNvs (Preferences round-trip) moved to the
// impure TimerSettingsApply.cpp: they touch Preferences, which this pure TU
// deliberately does not link.

void timerSettingsLoadDevJson(JsonObjectConst obj)
{
    for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
    {
        const TimerSettingDesc &d = TIMER_SETTINGS_DESCS[i];
        if (!obj.containsKey(d.devKey)) continue;
        TcValue val;
        if (timerSettingParse(d, obj[d.devKey], val))   // per-key best-effort: skip if invalid
            timerSettingStore(d, val);
    }
}

void timerSettingEmitValue(const TimerSettingDesc &d, JsonDocument &doc)
{
    switch (d.type)
    {
        case TcType::U16:  doc[d.cmdKey] = *static_cast<uint16_t *>(d.storage); break;
        case TcType::U32:  doc[d.cmdKey] = *static_cast<uint32_t *>(d.storage); break;
        case TcType::Bool: doc[d.cmdKey] = *static_cast<bool *>    (d.storage); break;
        case TcType::Str:  doc[d.cmdKey] = *static_cast<String *>  (d.storage); break;
    }
}

void timerSettingsBuildSnapshot(JsonDocument &doc)
{
    for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
    {
        const TimerSettingDesc &d = TIMER_SETTINGS_DESCS[i];
        if (!d.inSnapshot) continue;
        timerSettingEmitValue(d, doc);
    }
}

void timerSettingsCaptureSnapshot(TcValue out[])
{
    for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
    {
        const TimerSettingDesc &d = TIMER_SETTINGS_DESCS[i];
        if (!d.inSnapshot) continue;   // sync_* (local identity) excluded by construction
        switch (d.type)
        {
            case TcType::U16:  out[i].num = *static_cast<uint16_t *>(d.storage); break;
            case TcType::U32:  out[i].num = *static_cast<uint32_t *>(d.storage); break;
            case TcType::Bool: out[i].b   = *static_cast<bool *>    (d.storage); break;
            case TcType::Str:  out[i].str = *static_cast<String *>  (d.storage); break;
        }
    }
}

void timerSettingsRestoreSnapshot(const TcValue in[])
{
    for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
    {
        const TimerSettingDesc &d = TIMER_SETTINGS_DESCS[i];
        if (!d.inSnapshot) continue;
        timerSettingStore(d, in[i]);   // dispatch-by-type write; equality-skip return ignored
    }
}

const TimerSettingDesc *timerSettingByCmdKey(const char *cmdKey)
{
    for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
        if (strcmp(TIMER_SETTINGS_DESCS[i].cmdKey, cmdKey) == 0)
            return &TIMER_SETTINGS_DESCS[i];
    return nullptr;
}

// Inline RTTTL classifier/validator. Pure, no globals touched.
namespace
{
    // RTTTL tunes for a single timer alarm/tick are short; cap so a pathological
    // payload can't bloat the one-shot run state.
    constexpr size_t kInlineRtttlMaxLen = 256;

    // True iff the comma-separated control section carries at least one RTTTL default
    // token -- a token whose key is exactly d/o/b (duration/octave/beat). Checks the
    // token PREFIX (not a substring), so "foo=4" is not mistaken for an "o=" default.
    bool controlHasDefaultToken(const String &control)
    {
        int start = 0;
        const int n = control.length();
        while (start <= n)
        {
            int comma = control.indexOf(',', start);
            if (comma < 0) comma = n;
            String tok = control.substring(start, comma);
            tok.trim();
            tok.toLowerCase();
            if (tok.startsWith("d=") || tok.startsWith("o=") || tok.startsWith("b=")) return true;
            if (comma == n) break;
            start = comma + 1;
        }
        return false;
    }
}

bool timerMelodyIsInline(const String &s)
{
    // A bare melody file-name token is [A-Za-z0-9_-]* and never contains a colon;
    // an inline RTTTL tune always carries ':' separators. Content is the only signal.
    return s.indexOf(':') >= 0;
}

bool timerMelodyValidateInline(const String &s)
{
    if (s.length() == 0 || s.length() > kInlineRtttlMaxLen) return false;

    // RTTTL is name:control:notes -- exactly two colons (name may be empty).
    int c1 = s.indexOf(':');
    if (c1 < 0) return false;
    int c2 = s.indexOf(':', c1 + 1);
    if (c2 < 0) return false;
    if (s.indexOf(':', c2 + 1) >= 0) return false;   // a third colon is malformed

    String control = s.substring(c1 + 1, c2);
    String notes   = s.substring(c2 + 1);
    if (control.length() == 0 || notes.length() == 0) return false;

    // The control section must carry at least one RTTTL default token (d=/o=/b=).
    if (!controlHasDefaultToken(control)) return false;

    return true;
}

// Relocated out of TimerManager. Self-contained: inlines the
// h*3600+m*60+sec sum (timerHmsToSeconds below) rather than delegating, so the
// parse stays one readable pass -- behaviour-identical to the former static.
bool timerParseHMS(const String &in, uint32_t &outSeconds)
{
    String s = in;
    s.trim();
    if (s.length() == 0) return false;

    // Split on ':' into up to three numeric fields. Colon count decides units:
    // two colons = HH:MM:SS, one = MM:SS, none = bare seconds. Each field must be
    // a non-empty run of digits. Fields are summed without a 0-59 cap (carry).
    uint32_t fields[3] = {0, 0, 0};
    int count = 0;
    int start = 0;
    for (int i = 0; i <= s.length(); i++)
    {
        if (i == s.length() || s[i] == ':')
        {
            if (count >= 3) return false;          // more than two colons
            int len = i - start;
            if (len == 0) return false;            // empty field (e.g. "5:", ":30")
            uint32_t v = 0;
            for (int j = start; j < i; j++)
            {
                char c = s[j];
                if (c < '0' || c > '9') return false;  // non-numeric
                v = v * 10 + (uint32_t)(c - '0');
            }
            fields[count++] = v;
            start = i + 1;
        }
    }

    uint32_t h = 0, m = 0, sec = 0;
    if (count == 3)      { h = fields[0]; m = fields[1]; sec = fields[2]; }
    else if (count == 2) {                m = fields[0]; sec = fields[1]; }
    else                 {                                sec = fields[0]; }

    outSeconds = h * 3600UL + m * 60UL + sec;
    return true;
}

// Relocated out of TimerManager. Pure case-insensitive
// membership check over the three action verbs -- behaviour-identical.
bool timerIsValidAction(const String &s)
{
    String a = s; a.toLowerCase();
    return a == "start" || a == "pause" || a == "reset";
}

// Relocated out of TimerManager's statics so the command validator links the
// table family, not the singleton. Behaviour-identical to the former statics, all
// of which are gone (the thin forwarders were deleted).

bool timerIsValidIconName(const String &name)
{
    // Empty is valid (clears the icon). Otherwise [A-Za-z0-9_-], length cap 32.
    if (name.length() == 0) return true;
    if (name.length() > 32) return false;
    for (size_t i = 0; i < name.length(); ++i)
    {
        char c = name[i];
        bool ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
               || (c >= '0' && c <= '9') || c == '_' || c == '-';
        if (!ok) return false;
    }
    return true;
}

bool timerParseBuzzerMode(const String &s, BuzzerMode &out)
{
    uint8_t idx;
    if (!timerEnumParse(TIMER_BUZZER_CODEC, TIMER_BUZZER_CODEC_COUNT, s, idx)) return false;
    out = (BuzzerMode)idx;
    return true;
}

bool timerParseFinishedMode(const String &s, FinishedMode &out)
{
    uint8_t idx;
    if (!timerEnumParse(TIMER_FINISHED_CODEC, TIMER_FINISHED_CODEC_COUNT, s, idx)) return false;
    out = (FinishedMode)idx;
    return true;
}

String timerClock(uint32_t seconds, ClockStyle style)
{
    uint32_t h = seconds / 3600;
    uint32_t m = (seconds % 3600) / 60;
    uint32_t s = seconds % 60;
    char buf[16];
    switch (style)
    {
        case ClockStyle::Trimmed:
            if (h > 0) snprintf(buf, sizeof(buf), "%u:%02u:%02u", (unsigned)h, (unsigned)m, (unsigned)s);
            else       snprintf(buf, sizeof(buf), "%u:%02u",                 (unsigned)m, (unsigned)s);
            break;
        case ClockStyle::Padded:
            snprintf(buf, sizeof(buf), "%02u:%02u:%02u", (unsigned)h, (unsigned)m, (unsigned)s);
            break;
        case ClockStyle::Compact:
            if (seconds < 3600)       snprintf(buf, sizeof(buf), "%u:%02u",   (unsigned)(seconds / 60), (unsigned)s);
            else if (seconds < 36000) snprintf(buf, sizeof(buf), "%u:%02u",   (unsigned)h, (unsigned)m);
            else                      snprintf(buf, sizeof(buf), "%02u:%02u", (unsigned)h, (unsigned)m);
            break;
    }
    return String(buf);
}

String timerFormatHMS(uint32_t seconds)
{
    return timerClock(seconds, ClockStyle::Trimmed);
}

// Relocated out of TimerManager, the singleton's last pure statics
// (secondsToHMS / hmsToSeconds) -- byte-identical.
void timerSecondsToHMS(uint32_t sec, uint32_t &h, uint32_t &m, uint32_t &s)
{
    h = sec / 3600;
    m = (sec % 3600) / 60;
    s = sec % 60;
}

uint32_t timerHmsToSeconds(uint32_t h, uint32_t m, uint32_t s)
{
    return h * 3600UL + m * 60UL + s;
}

// ---------------------------------------------------------------------------
// HA attribute-group projection. The buzzer + finished
// HASelects were lit up first; the JSON-attributes opt-in then extended to
// HASensor, lighting up the remaining + state sensors. realert_interval is listed
// first on the finished carrier so the folded payload is a superset of the legacy
// bespoke {"realert_interval":N}. remaining_publish_interval rides BOTH the
// remaining sensor (its own cadence) and the state sensor (a complete config view)
// -- one settings row, two carrier rows. bar_color carries the per-row formatter
// so it renders as "default"/"#RRGGBB" rather than its raw-int snapshot form.
// max_duration is the first MULTI-CARRIER key whose
// representation differs PER CARRIER: it rides the Duration text entity in
// carrier-native clock form (formatMaxDurationHMS -> "24:00:00") AND the state
// sensor in raw seconds (no formatter -> 86400) -- one settings row, two carrier
// rows, two representations over the same persisted storage.
//   carrier, cmdKey, format
const TimerAttrGroupDesc TIMER_ATTR_GROUP_DESCS[] = {
    {TimerHaEntity::Duration,  "max_duration",               formatMaxDurationHMS},
    {TimerHaEntity::Finished,  "realert_interval",           nullptr},
    {TimerHaEntity::Finished,  "finished_hold",              nullptr},
    {TimerHaEntity::Buzzer,    "countdown_seconds",          nullptr},
    {TimerHaEntity::Buzzer,    "melody_tick",                nullptr},
    {TimerHaEntity::Buzzer,    "melody_end",                 nullptr},
    {TimerHaEntity::Remaining, "remaining_publish_interval", nullptr},
    {TimerHaEntity::State,     "max_duration",               nullptr},
    {TimerHaEntity::State,     "remaining_publish_interval", nullptr},
    {TimerHaEntity::State,     "icon_enabled",               nullptr},
    {TimerHaEntity::State,     "bar_enabled",                nullptr},
    {TimerHaEntity::State,     "bar_color",                  timerFormatBarColor},
    {TimerHaEntity::State,     "bar_bg_color",               timerFormatBarBgColor},
    {TimerHaEntity::State,     "sync_follow",                nullptr},
    {TimerHaEntity::State,     "sync_targets",               nullptr},
};

const size_t TIMER_ATTR_GROUP_DESC_COUNT =
    sizeof(TIMER_ATTR_GROUP_DESCS) / sizeof(TIMER_ATTR_GROUP_DESCS[0]);

void timerBuildAttributeGroup(TimerHaEntity carrier, JsonDocument &doc)
{
    for (size_t i = 0; i < TIMER_ATTR_GROUP_DESC_COUNT; ++i)
    {
        const TimerAttrGroupDesc &g = TIMER_ATTR_GROUP_DESCS[i];
        if (g.carrier != carrier) continue;
        const TimerSettingDesc *d = timerSettingByCmdKey(g.cmdKey);
        if (!d) continue;   // table self-consistency is pinned by test T14
        if (g.format) g.format(*d, doc);
        else          timerSettingEmitValue(*d, doc);
    }
}

// ===========================================================================
// Member-backed config half (B1) -- PURE validation projection.
// The validate predicates + the parallel {cmdKey, validate} table
// (TIMER_MEMBER_VALIDATORS) that TimerCommand::classify uses live HERE, with no
// singleton/MQTT dependency. The impure half -- the apply/emit/publish hooks, the
// full TIMER_MEMBER_CONFIG_DESCS table, and its snapshot/publish helpers -- lives in
// TimerSettingsApply.cpp (they route through TimerManager's setters and the MQTT
// seam). Both tables reference the SAME validate function pointers below, so a drift
// guard test pins them row-for-row: they cannot disagree about a member key.
// ===========================================================================
bool timerMemValidateBuzzer(JsonVariantConst v, TcValue &out)
{
    BuzzerMode m;
    if (!timerParseBuzzerMode(v.as<String>(), m)) return false;
    out.num = (uint32_t)m;
    return true;
}

bool timerMemValidateFinished(JsonVariantConst v, TcValue &out)
{
    FinishedMode m;
    if (!timerParseFinishedMode(v.as<String>(), m)) return false;
    out.num = (uint32_t)m;
    return true;
}

bool timerMemValidateIcon(JsonVariantConst v, TcValue &out)
{
    String s = v.as<String>();
    if (!timerIsValidIconName(s)) return false;
    out.str = s;
    return true;
}

const TimerMemberValidatorDesc TIMER_MEMBER_VALIDATORS[] = {
    {"buzzer",        timerMemValidateBuzzer},
    {"finished",      timerMemValidateFinished},
    {"icon_idle",     timerMemValidateIcon},
    {"icon_running",  timerMemValidateIcon},
    {"icon_paused",   timerMemValidateIcon},
    {"icon_finished", timerMemValidateIcon},
};

const size_t TIMER_MEMBER_VALIDATOR_COUNT =
    sizeof(TIMER_MEMBER_VALIDATORS) / sizeof(TIMER_MEMBER_VALIDATORS[0]);

static_assert(sizeof(TIMER_MEMBER_VALIDATORS) / sizeof(TIMER_MEMBER_VALIDATORS[0]) == TIMER_MEMBER_CONFIG_DESC_CAP,
              "TIMER_MEMBER_CONFIG_DESC_CAP must equal the member-config row count");
#endif // AWTRIX_DISABLE_TIMER

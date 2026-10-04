#ifndef TimerSettings_h
#define TimerSettings_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "TimerHa.h"      // TimerHaEntity (the HA carrier each attribute group rides)
#include "TimerEnums.h"   // BuzzerMode / FinishedMode for the relocated enum parsers

// Persisted Timer settings: the single descriptor table that drives validation,
// apply, NVS persistence, dev.json overrides and the propagated config snapshot
// for the value-config Timer keys. Modelled on TimerHa.h ("one table, no drift").
//
// This table is a SUPERSET, not "the config block": the `inSnapshot` column
// codifies the config/identity boundary --
//   * config block            = the inSnapshot == true rows (the propagated snapshot)
//   * sync roles / local id    = the inSnapshot == false rows (sync_follow/sync_targets,
//                                deliberately excluded from the snapshot)
//
// Out of scope (the B1 boundary): duration/buzzer/finished and the
// four icon_<state> keys stay on TimerManager's publish-aware setters. They are config
// block too, but their snapshot/broadcast membership is governed by the shared
// member-config list in TimerManager.cpp, not by this table.

enum class TcType  : uint8_t { U16, U32, Bool, Str };
enum class TcCheck : uint8_t { Bool, UIntRange, Name, Bespoke };

// A validated + coerced value, staged before any global is written so parseCommand
// keeps its atomic-reject contract (validate everything, then apply).
struct TcValue
{
    uint32_t num = 0;
    bool     b   = false;
    String   str;
};

struct TimerSettingDesc
{
    const char *cmdKey;     // POST/MQTT + snapshot key, e.g. "finished_hold"
    const char *devKey;     // dev.json key,            e.g. "timer_finished_hold"
    const char *nvsKey;     // NVS "awtrix" key,        e.g. "TFHOLD"
    TcType      type;
    TcCheck     check;
    uint32_t    lo, hi;     // UIntRange bounds (ranges live HERE, once)
    uint32_t    dfltNum;    // NVS default for U16/U32/Bool
    const char *dfltStr;    // NVS default for Str; also the Name empty-substitution value
    // Bespoke validators (bar_color / sync_targets only); nullptr for declarative rows.
    bool      (*bespoke)(JsonVariantConst, TcValue &out);
    bool        inSnapshot; // member of the propagated config block iff true
    void       *storage;    // typed by `type`: uint16_t* / uint32_t* / bool* / String*
};

extern const TimerSettingDesc TIMER_SETTINGS_DESCS[];
extern const size_t           TIMER_SETTINGS_DESC_COUNT;

// Compile-time row count (TIMER_SETTINGS_DESC_COUNT is only known at runtime, so it
// cannot size a fixed array). A static_assert in TimerSettings.cpp pins it equal to
// the table extent, so it cannot drift. Used by the one-shot override controller
// to stack a config snapshot buffer over the table.
constexpr size_t TIMER_SETTINGS_DESC_CAP = 13;

// Validate + coerce one field into `out`. Never writes a global (pure parse). Strict
// JSON types match the legacy parseCommand: numbers reject bool/string/null; bools
// require a real JSON bool. Returns false on any violation.
bool timerSettingParse(const TimerSettingDesc &d, JsonVariantConst v, TcValue &out);

// Write a previously-parsed value to the descriptor's storage (dispatch on type).
// Equality-skip: returns true iff the stored value actually changed, so callers
// can elide the "awtrix" flush for no-op writes (mirrors the member setters).
bool timerSettingStore(const TimerSettingDesc &d, const TcValue &v);

// NVS round-trip for the whole table (caller brackets begin()/end()).
void timerSettingsLoadNvs(class Preferences &prefs);
void timerSettingsSaveNvs(class Preferences &prefs);

// dev.json overrides: per-key best-effort (validate each present devKey, store iff
// valid, skip-and-continue otherwise -- NOT atomic; dev.json is a boot override layer).
void timerSettingsLoadDevJson(JsonObjectConst obj);

// Emit ONE row's live value into `doc` under its cmdKey, dispatched by `type` and
// IGNORING snapshot membership. The single "storage -> JSON value" helper shared by
// the config snapshot (inSnapshot rows) and the HA attribute builder, so the two can
// never disagree about a value.
void timerSettingEmitValue(const TimerSettingDesc &d, JsonDocument &doc);

// Emit the inSnapshot rows into `doc` (the propagated config block, table half).
void timerSettingsBuildSnapshot(JsonDocument &doc);

// One-shot override: capture/restore the table half's config
// block (the inSnapshot rows; sync_* are inSnapshot=false and excluded by
// construction) into a caller-owned TcValue buffer of TIMER_SETTINGS_DESC_CAP slots,
// indexed by row. Capture reads each storage by type; restore writes it back via
// timerSettingStore. Generic over the table, so no per-key code is required.
void timerSettingsCaptureSnapshot(TcValue out[]);
void timerSettingsRestoreSnapshot(const TcValue in[]);

// Lookup by command key (used by MenuManager to reuse a row's range bounds).
const TimerSettingDesc *timerSettingByCmdKey(const char *cmdKey);

// ---------------------------------------------------------------------------
// Inline RTTTL classifier/validator -- a pure, reusable pair
// reused by command validation and melody resolution. `melody_end`/`melody_tick`
// accept EITHER a bare file-name token (as today) OR an inline RTTTL tune; the two
// are distinguished by content. An inline tune is always one-shot (it has no
// persistable file form), so it is never written to a melody name global.

// True iff `s` is an inline RTTTL tune rather than a bare melody file-name token.
// A bare token is [A-Za-z0-9_-]* (never contains a colon); an inline tune carries
// RTTTL structure, so the presence of a ':' is the discriminator.
bool timerMelodyIsInline(const String &s);

// Validate an inline RTTTL tune's syntax: `name:control:notes` (exactly two colons),
// a non-empty control section carrying a d=/o=/b= token, a non-empty notes section,
// within a length cap. Returns false on any violation (a malformed inline tune is
// BadField/400, atomic-reject). Name may be empty.
bool timerMelodyValidateInline(const String &s);

// ---------------------------------------------------------------------------
// Pure validation helpers relocated out of TimerManager's statics so the
// command validator links the descriptor-table family, not the singleton. The
// singleton's thin forwarders were deleted once production callers were gone;
// these free functions are the only spellings.

// Parse a duration string into seconds. Accepts bare seconds, MM:SS or HH:MM:SS;
// each field is a non-empty digit run; fields are summed without a 0-59 cap
// (carry); surrounding whitespace is trimmed. No clamp, no globals. Returns false
// (leaving outSeconds untouched) on empty input, an empty field, more than two
// colons, or a non-numeric field.
bool timerParseHMS(const String &s, uint32_t &outSeconds);

// True iff `s` is a valid timer action verb -- case-insensitive, exactly one of
// {start, pause, reset}. No trim (a surrounding space rejects). No globals.
bool timerIsValidAction(const String &s);

// More pure validators/formatters relocated into the table family so the
// command validator (TimerCommand::classify) links the family, not the singleton.

// Bare icon/melody file-name char-rule: [A-Za-z0-9_-], length 0..32 (empty = clear,
// accepted). The validation predicate behind the table's Name check and
// TimerManager's private icon-slot validation. No globals.
bool timerIsValidIconName(const String &name);

// String->enum over the buzzer / finished codec tables, case-insensitive
// wire spelling or alias. The enum parse behind the member validators. No globals.
bool timerParseBuzzerMode(const String &s, BuzzerMode &out);
bool timerParseFinishedMode(const String &s, FinishedMode &out);

// The one seconds->clock renderer. Three deliberate spellings the timer
// surfaces speak, collapsed into one function:
//   Trimmed  drop the hours group when zero; most-significant field unpadded, lower
//            fields zero-padded ("24:00:00" / "1:00:00" / "0:45"). HA/config-read form.
//   Padded   always zero-padded HH:MM:SS ("24:00:00" / "01:00:00" / "00:00:45"). Menu
//            / config-screen form.
//   Compact  two segments, seconds dropped past the hour mark: M:SS (<1h), H:MM (<10h),
//            HH:MM (>=10h). The running-display form. No globals.
enum class ClockStyle { Trimmed, Padded, Compact };
String timerClock(uint32_t seconds, ClockStyle style);

// Trimmed clock string ("24:00:00" / "1:00:00" / "0:45"). Thin alias for
// timerClock(seconds, ClockStyle::Trimmed); the Duration wire/state spelling
// (state JSON, Duration publishes/echoes, the max_duration HA formatter). No globals.
String timerFormatHMS(uint32_t seconds);

// Raw seconds <-> H/M/S integer decomposition, relocated out of TimerManager's
// last pure statics. Pure math, no clamp (the on-device config editor —
// the one caller — owns its 99h display cap), no globals.
void     timerSecondsToHMS(uint32_t sec, uint32_t &h, uint32_t &m, uint32_t &s);
uint32_t timerHmsToSeconds(uint32_t h, uint32_t m, uint32_t s);

// ---------------------------------------------------------------------------
// HA attribute-group projection -- the descriptor-table family's fifth
// member. One row per (carrier entity, settings key) projection: a persisted
// settings value surfaced as a read-only JSON attribute on the HA entity it is
// semantically about. A key may map to MULTIPLE carriers (one row each). The
// optional `format` hook overrides the raw emit for rows whose attribute
// representation deliberately differs from the config-snapshot one (e.g.
// bar_color's "#RRGGBB" string); nullptr means emit the raw live value via
// timerSettingEmitValue. One table, one generic builder -- adding or moving a
// knob's HA projection is a one-row change with no drift across publish paths.
struct TimerAttrGroupDesc
{
    TimerHaEntity carrier;   // the HA entity that carries this attribute
    const char   *cmdKey;    // joins TIMER_SETTINGS_DESCS by cmdKey (the attribute key, verbatim)
    void        (*format)(const TimerSettingDesc &d, JsonDocument &doc);  // nullptr = raw emit
};

extern const TimerAttrGroupDesc TIMER_ATTR_GROUP_DESCS[];
extern const size_t             TIMER_ATTR_GROUP_DESC_COUNT;

// Build one carrier's attribute object into `doc`: every table row mapped to
// `carrier`, emitted under its cmdKey (via the row's formatter, else the shared
// timerSettingEmitValue). `doc` is left empty for a carrier with no mapped rows.
void timerBuildAttributeGroup(TimerHaEntity carrier, JsonDocument &doc);

// ---------------------------------------------------------------------------
// The config block's SECOND table: the member-backed half (B1).
//
// One row per member-backed config key (buzzer / finished / the four icon_<state>).
// Unlike TIMER_SETTINGS_DESCS' DECLARATIVE rows, these carry function-pointer hooks
// -- exactly like TIMER_MENU_SLOTS' enum slots -- because their values are owned by
// TimerManager's publish-aware setters (equality-skip, _suspendPersist batching, MQTT
// publish), not by a typed storage pointer. This is deliberately NOT folded
// into the declarative table; it is a separate hook table, the fourth member of the
// descriptor-table family (TIMER_SETTINGS_DESCS, TIMER_HA_DESCRIPTORS, TIMER_MENU_SLOTS).
//
// Together the two tables ARE the config block. `duration` is member-backed too but is
// deliberately EXCLUDED: it is run-state, not config, and its validation
// depends on the cross-field effectiveMaxDuration staged from the table half.
struct TimerMemberConfigDesc
{
    const char *cmdKey;                                  // POST/MQTT + snapshot key
    bool (*validate)(JsonVariantConst, TcValue &out);    // pure: validate + coerce, no mutation
    void (*apply)(const TcValue &);                      // routes via TimerManager's deep setter
    void (*emit)(JsonDocument &doc);                     // writes the live value into the snapshot
    void (*publish)();                                   // optional: live value onto the MQTT wire
                                                         // seam; nullptr = key not
                                                         // individually published
};

extern const TimerMemberConfigDesc TIMER_MEMBER_CONFIG_DESCS[];
extern const size_t                TIMER_MEMBER_CONFIG_DESC_COUNT;

// Compile-time row count for the member-config half (the runtime COUNT can't size a
// fixed array), used by TimerCommand::Plan to stage member values. A static_assert
// pins it to the table extent. Bump if a member-config row is added.
constexpr size_t TIMER_MEMBER_CONFIG_DESC_CAP = 6;

// The member-config half's PURE validation projection. TIMER_MEMBER_CONFIG_DESCS
// carries impure apply/emit/publish hooks (the TimerManager singleton + the MQTT wire
// seam), so the whole table can't link where only the dependency-light command
// validator is wanted. This parallel table carries ONLY the pure {cmdKey, validate}
// columns -- the exact same validate function pointers -- so TimerCommand::classify
// validates the member half without dragging in the apply machinery. The two
// tables stay row-for-row (same cmdKey, same validate) so they cannot
// disagree about what a member key is or how it validates.
struct TimerMemberValidatorDesc
{
    const char *cmdKey;
    bool (*validate)(JsonVariantConst, TcValue &out);   // pure: validate + coerce, no mutation
};

extern const TimerMemberValidatorDesc TIMER_MEMBER_VALIDATORS[];
extern const size_t                   TIMER_MEMBER_VALIDATOR_COUNT;

// The member-config validate predicates (pure; live in TimerSettings.cpp). Declared
// so BOTH tables reference the SAME function pointers: TIMER_MEMBER_VALIDATORS (pure
// half) and TIMER_MEMBER_CONFIG_DESCS' validate column (impure half). Sharing the
// pointer is what makes the drift guard exact -- the two tables cannot validate a key
// differently because they validate it with the same function.
bool timerMemValidateBuzzer(JsonVariantConst v, TcValue &out);
bool timerMemValidateFinished(JsonVariantConst v, TcValue &out);
bool timerMemValidateIcon(JsonVariantConst v, TcValue &out);

// bar_color / bar_bg_color attribute formatters (external linkage, defined in the pure
// TU) shared by the attr-group table and the HTTP full-config dump.
void timerFormatBarColor(const TimerSettingDesc &d, JsonDocument &doc);
void timerFormatBarBgColor(const TimerSettingDesc &d, JsonDocument &doc);

// Emit each member-config key's live value into `doc` (the B1 half of the config snapshot).
void timerMemberConfigBuildSnapshot(JsonDocument &doc);

// Run `cmdKey`'s declared publish hook (no-op if the key has none / is unknown).
// TimerManager's per-key publish methods dispatch through this, so the row is the
// single place "how key X goes out on the wire" is defined.
void timerMemberConfigPublish(const char *cmdKey);

// ---------------------------------------------------------------------------
// HTTP GET /api/timer config mirror. Build the COMPLETE persisted
// configuration into `doc`: the full two-table dump an HTTP-only client reads
// back so it can confirm everything it can POST. Unlike timerSettingsBuildSnapshot
// it does NOT honour the inSnapshot filter -- the sync-role keys
// (sync_follow/sync_targets) ARE included -- and it appends the member-config half
// (buzzer/finished/the four icon_*) via timerMemberConfigBuildSnapshot. Pure: it
// reads only the descriptor storage, no I/O, no globals beyond that. Because it
// walks the same two tables that drive the control surface, the read surface
// cannot drift from what is writable. Values are raw here; the two
// carrier-native renderings (friendly bar_color, max_duration_str) are layered
// on by the HA attribute builder.
void timerBuildFullConfig(JsonDocument &doc);

#endif

#ifndef AWTRIX_DISABLE_TIMER
#include "TimerCommand.h"

#include <string.h>   // strcmp

// The pure Timer command plan. See TimerCommand.h. Lifted verbatim
// (behaviour-identical) from TimerManager::parseCommand's validation pass; the
// shell now drives apply from the returned Plan instead of re-deriving these flags.

namespace TimerCommand
{
    Plan classify(JsonObjectConst packet, const Context &ctx)
    {
        Plan p;   // ok defaults false; arrays default-zeroed

        // -- Table settings: validate + coerce each present row into staging. Nothing is
        //    kept until every field below validates too (atomic-reject).
        //    max_duration is range-defining for duration: capture its staged value so a
        //    payload that raises the ceiling AND sets a duration within it in the same
        //    call is accepted atomically. melody_end/melody_tick
        //    accept an inline RTTTL tune (detected by content): validated here, staged as
        //    RAM, and the table row excluded from the bare-name store (always one-shot).
        uint32_t effectiveMaxDuration = ctx.savedMaxDuration;
        for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
        {
            const TimerSettingDesc &d = TIMER_SETTINGS_DESCS[i];
            p.tablePresent[i] = packet.containsKey(d.cmdKey);
            if (!p.tablePresent[i]) continue;

            bool isMelodyKey = (strcmp(d.cmdKey, "melody_end") == 0 || strcmp(d.cmdKey, "melody_tick") == 0);
            if (isMelodyKey)
            {
                String mv = packet[d.cmdKey].as<String>();
                if (timerMelodyIsInline(mv))
                {
                    if (!timerMelodyValidateInline(mv)) return p;   // !ok
                    if (strcmp(d.cmdKey, "melody_end") == 0) { p.inlineEnd = mv;  p.haveInlineEnd = true; }
                    else                                     { p.inlineTick = mv; p.haveInlineTick = true; }
                    p.tablePresent[i] = false;   // excluded from the bare-name parse/store/snapshot
                    continue;
                }
            }

            if (!timerSettingParse(d, packet[d.cmdKey], p.tableStaged[i])) return p;   // !ok
            if (strcmp(d.cmdKey, "max_duration") == 0) effectiveMaxDuration = p.tableStaged[i].num;
        }

        // -- duration (member-backed, B1): validated against the effective ceiling above.
        p.haveDuration = packet.containsKey("duration");
        if (p.haveDuration)
        {
            JsonVariantConst dv = packet["duration"];
            if (dv.is<const char *>())
            {
                if (!timerParseHMS(dv.as<String>(), p.durationSec)) return p;   // !ok
            }
            else if (dv.is<long>() || dv.is<float>())   // any JSON number; not bool/object/null
            {
                p.durationSec = dv.as<uint32_t>();
            }
            else
            {
                return p;   // !ok
            }
            if (p.durationSec < 1 || (effectiveMaxDuration > 0 && p.durationSec > effectiveMaxDuration))
                return p;   // !ok
        }

        // -- Member-backed config half (B1): validate + coerce each present row via its
        //    pure validator. A bad member field rejects the whole command.
        for (size_t i = 0; i < TIMER_MEMBER_VALIDATOR_COUNT; ++i)
        {
            const TimerMemberValidatorDesc &d = TIMER_MEMBER_VALIDATORS[i];
            p.memberPresent[i] = packet.containsKey(d.cmdKey);
            if (!p.memberPresent[i]) continue;
            if (!d.validate(packet[d.cmdKey], p.memberStaged[i])) return p;   // !ok
        }

        // -- action (run-state verb).
        if (packet.containsKey("action"))
        {
            String a = packet["action"].as<String>();
            if (!timerIsValidAction(a)) return p;   // !ok
            a.toLowerCase();
            p.action = (a == "start") ? Action::Start
                     : (a == "pause") ? Action::Pause
                                      : Action::Reset;
        }

        // -- save flag: payload-level bool, default true. A
        //    non-boolean rejects the whole command (atomic-reject).
        bool saveFlag = true;
        if (packet.containsKey("save"))
        {
            JsonVariantConst sv = packet["save"];
            if (!sv.is<bool>()) return p;   // !ok
            saveFlag = sv.as<bool>();
        }

        // -- one-shot decision. An inline melody is always one-shot; a
        //    remote-applied command is ALWAYS one-shot regardless of the leader's save.
        p.oneShot = !saveFlag || p.haveInlineEnd || p.haveInlineTick || ctx.remoteApply;

        // -- configInCommand: a config-block key present (table inSnapshot half OR the
        //    member-backed half). sync_* (inSnapshot=false) and action/duration
        //    (run-state) are excluded. Drives rebaseline + config broadcast.
        bool snapshotChanged = false;
        for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
            if (p.tablePresent[i] && TIMER_SETTINGS_DESCS[i].inSnapshot) snapshotChanged = true;
        bool memberTouched = false;
        for (size_t i = 0; i < TIMER_MEMBER_VALIDATOR_COUNT; ++i)
            if (p.memberPresent[i]) memberTouched = true;
        p.configInCommand = snapshotChanged || memberTouched;

        // -- HA attribute carriers dirtied by this command (any mapped key present).
        //    Table-driven, fires for any present key including sync_* (State carrier),
        //    exactly as the old apply loop. The shell republishes these iff !oneShot.
        for (size_t i = 0; i < TIMER_ATTR_GROUP_DESC_COUNT; ++i)
        {
            const TimerAttrGroupDesc &g = TIMER_ATTR_GROUP_DESCS[i];
            if (packet.containsKey(g.cmdKey)) p.attrCarrierDirty[(size_t)g.carrier] = true;
        }

        p.ok = true;
        return p;
    }
}
#endif // AWTRIX_DISABLE_TIMER

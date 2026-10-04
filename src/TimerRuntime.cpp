#ifndef AWTRIX_DISABLE_TIMER
#include "TimerRuntime.h"

namespace TimerRuntime
{
    // The Finished transition's effect bundle. Ported straight from the old
    // TimerManager::enterFinished, in the SAME order the imperative code emitted:
    // publishState, publishRemaining, then the three gated effects (switch-to-app,
    // brightness restore, end tone). Keeping the order byte-identical is what lets
    // the adapter stay behaviourally indistinguishable on device.
    static void appendFinished(std::vector<Effect> &fx, const Inputs &in)
    {
        fx.push_back({EffectKind::PublishState});
        fx.push_back({EffectKind::PublishRemaining});
        if (in.navigationFree)
            fx.push_back({EffectKind::SwitchToTimerApp});
        if (in.matrixOff)
        {
            Effect e{EffectKind::SetBrightness};
            e.brightness = in.brightness;
            fx.push_back(e);
        }
        if (in.playEndTone)
        {
            Effect e{EffectKind::PlayTone};
            e.tone = Tone::End;
            fx.push_back(e);
        }
    }

    // The reset bundle, shared by an explicit reset and a paused-duration edit:
    // stop any sound, republish the (now Idle) run-state, and — when the panel is
    // dark — drop brightness to 0. Ported in order from the old TimerManager::reset.
    static void appendReset(std::vector<Effect> &fx, const Inputs &in)
    {
        fx.push_back({EffectKind::StopSound});
        fx.push_back({EffectKind::PublishState});
        fx.push_back({EffectKind::PublishRemaining});
        if (in.matrixOff)
            fx.push_back({EffectKind::SetBrightness});   // brightness 0
    }

    // The input-driven lifecycle transitions: start/pause/reset/
    // setDuration decided as pure phase transitions over the same effect seam. The
    // adapter owns the run-state bookkeeping each Transition names (enterRunning's
    // runStart* capture, the one-shot override restore behind ToIdle); here we only
    // decide the phase change and the ordered effects.
    static Result stepCommand(const State &s, const Inputs &in)
    {
        Result r;
        switch (in.command)
        {
        case Command::Start:
            if (s.phase == TimerState::Running) break;   // already running: no-op
            if (s.phase == TimerState::Finished)
                r.effects.push_back({EffectKind::StopSound});
            r.effects.push_back({EffectKind::PublishState});
            r.effects.push_back({EffectKind::PublishRemaining});
            r.transition = Transition::ToRunning;
            break;

        case Command::Pause:
            if (s.phase == TimerState::Running)
            {
                r.effects.push_back({EffectKind::PublishState});
                r.effects.push_back({EffectKind::PublishRemaining});
                r.transition = Transition::ToPaused;
            }
            else if (s.phase == TimerState::Paused)
            {
                // A second press resumes: same publishes as enterRunning.
                r.effects.push_back({EffectKind::PublishState});
                r.effects.push_back({EffectKind::PublishRemaining});
                r.transition = Transition::ToRunning;
            }
            break;

        case Command::Reset:
            appendReset(r.effects, in);
            r.transition = Transition::ToIdle;
            break;

        case Command::SetDuration:
            if (s.phase == TimerState::Idle)
            {
                r.effects.push_back({EffectKind::PublishRemaining});
                r.transition = Transition::IdleReload;
            }
            else if (s.phase == TimerState::Paused)
            {
                // US6: editing the duration while Paused resets cleanly to Idle
                // with the new duration — the reset bundle, then remaining reloads.
                appendReset(r.effects, in);
                r.transition = Transition::ToIdle;
            }
            // Running / Finished: duration changes silently (the adapter still
            // persists + republishes it); no run-state transition.
            break;

        case Command::Tick:
            break;   // unreachable: step() dispatches Tick to the countdown path
        }
        return r;
    }

    Result step(const State &state, unsigned long nowMs, const Inputs &in)
    {
        if (in.command != Command::Tick)
            return stepCommand(state, in);

        Result r;

        if (state.phase == TimerState::Finished)
        {
            if (in.finishedMode == FinishedMode::AutoClear &&
                (nowMs - state.enteredFinishedMs >= (unsigned long)in.finishedHold * 1000UL))
            {
                // Auto-clear: stop the sound, return to Idle, republish, and (when
                // the panel is dark) drop brightness to 0. The adapter maps ToIdle
                // to returnToIdle()+state/remaining so the override store stays there.
                r.effects.push_back({EffectKind::StopSound});
                r.effects.push_back({EffectKind::PublishState});
                r.effects.push_back({EffectKind::PublishRemaining});
                if (in.matrixOff)
                    r.effects.push_back({EffectKind::SetBrightness});   // brightness 0
                r.transition = Transition::ToIdle;
            }
            else if (in.finishedMode == FinishedMode::ReAlert && in.realertArmed &&
                     (nowMs - state.lastRealertMs >= (unsigned long)in.realertInterval * 1000UL))
            {
                // Re-sound the end tone at the interval. The anchor advances even
                // when a tone is already playing (one tone at a time), so the next
                // re-alert lands a full interval later — not immediately after.
                if (!in.isPlaying && in.realertToneSet)
                {
                    Effect e{EffectKind::PlayTone};
                    e.tone = Tone::End;
                    r.effects.push_back(e);
                }
                r.realertFired = true;
            }
            return r;
        }

        if (state.phase != TimerState::Running)
            return r;

        if (in.newRemaining != state.remainingSec)
        {
            if (in.countdownArmed)
            {
                // Beep if any second in [1, countdownSeconds] was crossed this
                // tick. The buzzer plays one tone at a time, so a single beep
                // covers the gap when tick() falls behind (rather than queuing N
                // back-to-back plays).
                uint32_t lo = in.newRemaining > 0 ? in.newRemaining : 1;
                uint32_t hi = state.remainingSec > 0 ? state.remainingSec - 1 : 0;
                if (hi > in.countdownSeconds) hi = in.countdownSeconds;
                if (hi >= lo && !in.isPlaying)
                {
                    Effect e{EffectKind::PlayTone};
                    e.tone = Tone::Tick;
                    r.effects.push_back(e);
                }
            }

            if (in.publishInterval > 0 &&
                (nowMs - state.lastPublishMs >= (unsigned long)in.publishInterval * 1000UL))
            {
                r.effects.push_back({EffectKind::PublishRemaining});
                r.publishFired = true;
            }
        }

        if (in.newRemaining == 0)
        {
            appendFinished(r.effects, in);
            r.transition = Transition::ToFinished;
        }
        return r;
    }
}
#endif // AWTRIX_DISABLE_TIMER

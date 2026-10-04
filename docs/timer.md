# Timer

AWTRIX 3 has a built-in countdown **Timer app**: set a duration, start it, and the
clock counts down with a progress bar, finishing with a melody and a blinking
`0:00` screen. It is controllable four ways, all driving the same validated
command surface:

- **On device** — buttons inside the Timer app, plus the `TIMER` item in the
  [onscreen menu](onscreen.md).
- **HTTP** — `POST /api/timer` (see the [API reference](api.md#timer-control)).
- **MQTT** — the `{prefix}/timer` command topic (same JSON as HTTP).
- **Home Assistant** — auto-discovered entities (below).

The app's display, states, physical buttons and buzzer/finished modes are
documented in [Apps](apps.md#timer). The full command/key reference with
validation rules lives in the [API docs](api.md#timer-control); the `dev.json`
boot overrides in [dev.md](dev.md). This page covers the quick start, the Home
Assistant surface, and multi-device sync.

The Timer ships enabled; disable it entirely with `show_timer` in `dev.json` or
the `TIMER` key on `/api/settings` (this removes the app, unpublishes the HA
entities and ignores timer commands).

## Quick start

Start a 10-minute timer:

```bash
curl -X POST http://[IP]/api/timer -H "Content-Type: application/json" \
     -d '{"duration":"10:00","action":"start"}'
```

or publish the same JSON to `{prefix}/timer`. Pause, resume and reset:

```json
{"action":"pause"}
{"action":"start"}
{"action":"reset"}
```

Durations accept `"HH:MM:SS"`, `"MM:SS"` or bare seconds (`600`). Colon count
decides the units (`"3:00"` is 3 minutes), out-of-range fields carry
(`"3:90"` = 270 s). Malformed or out-of-range input is **rejected**
(HTTP `400`; MQTT silently ignores it) — never clamped, and an invalid command
changes nothing at all (atomic reject).

Retained state topics `{prefix}/stats/timer_state`, `…/timer_dur` and
`…/timer_rem` report the lifecycle state (`idle`/`running`/`paused`/`finished`),
the configured duration as a clock string, and the seconds remaining (published
every `remaining_publish_interval` seconds while running).

### One-shot runs

Add `save:false` to run a timer once with custom settings — the config applies
to that run only, writes nothing to flash, and reverts to your saved settings
when the timer returns to Idle:

```json
{"duration":"0:30","buzzer":"end","melody_end":"beep:d=16,o=6,b=180:c,c,c","action":"start","save":false}
```

`melody_end`/`melody_tick` accept either a bare melody name (a file under
`/MELODIES/`) or an inline RTTTL tune as above; an inline tune is always
one-shot. While a one-shot run is active, every observation surface (the
`GET /api/timer` config mirror, the HA attributes, peer sync) keeps reporting
your **saved** configuration. Details in the [API docs](api.md#timer-control).

## Home Assistant entities

With `HA_DISCOVERY=true` the firmware advertises ten Timer entities:

| Entity | Type | Purpose |
| --- | --- | --- |
| `{id}_timer_dur`   | `text`   | Duration as a clock string (writable; accepts `MM:SS`/bare seconds too). Invalid input reverts to the previous value. |
| `{id}_timer_rem`   | `sensor` | Seconds remaining (read-only; updates every `remaining_publish_interval` s while running). |
| `{id}_timer_state` | `sensor` | `idle` / `running` / `paused` / `finished`. |
| `{id}_timer_buz`   | `select` | Buzzer mode (`Off` / `End` / `Countdown`). |
| `{id}_timer_fin`   | `select` | Finished mode (`Clear` / `Hold` / `Re-alert`). |
| `{id}_timer_start` | `button` | Equivalent to `{"action":"start"}`. |
| `{id}_timer_pause` | `button` | Equivalent to `{"action":"pause"}`. |
| `{id}_timer_reset` | `button` | Equivalent to `{"action":"reset"}`. |
| `{id}_timer_sync_follow`  | `switch` | Multi-device sync receive consent (`sync_follow`), writable. |
| `{id}_timer_sync_targets` | `select` | Multi-device sync send targeting (`sync_targets`), writable: `Off`, `All`, plus one option per **discovered peer clock** (the list updates as peers appear/disappear). A multi-ID CSV set over HTTP/MQTT shows as unknown; the state sensor's `sync_targets` attribute stays authoritative. |

When a timer is started from Idle the display auto-switches to the Timer app
(unless a game or a navigation-blocking app is active).

### Read-only configuration attributes

Each carrier entity also exposes the persisted settings it is semantically
about as a **read-only JSON attribute object**, so the device's configuration
is readable from inside HA without an MQTT/HTTP query:

| Carrier | Attributes |
| --- | --- |
| `{id}_timer_dur`   | `max_duration` (clock form, e.g. `"24:00:00"`) |
| `{id}_timer_rem`   | `remaining_publish_interval` |
| `{id}_timer_buz`   | `countdown_seconds`, `melody_tick`, `melody_end` |
| `{id}_timer_fin`   | `realert_interval`, `finished_hold` |
| `{id}_timer_state` | the full config: `max_duration` (raw seconds), `remaining_publish_interval`, `icon_enabled`, `bar_enabled`, `bar_color` (`"default"` or `"#RRGGBB"`), `bar_bg_color` (`"none"` or `"#RRGGBB"`), `sync_follow`, `sync_targets` |

The attribute objects are retained and republished after every config change
and MQTT (re)connect. During a one-shot run they keep reporting the saved
configuration, never the transient one-off values.

## Multi-device sync

Two or more AWTRIX clocks on the same LAN can mirror each other's timer: when
one starts, pauses or resets, the action propagates to a chosen set of peers.
Sync is **broker-free** — it rides UDP broadcast on port **4212**, so it works
without MQTT.

### Roles: two independent settings

| Setting | Axis | Meaning |
| --- | --- | --- |
| `sync_targets` | send | Whom this clock commands on a *local* action: `""` (send nothing, the default), `all`, or a comma list of peer `uniqueID`s. |
| `sync_follow`  | receive | Whether this clock obeys inbound sync it is targeted by (default **on**; turn off to make a clock ignore peers). |

Compose them freely: a **leader** (targets set, follow off) commands but never
obeys; a **follower** (follow on, no targets) obeys but never commands;
set every clock to `sync_targets=all` + `sync_follow=true` for "everyone
mirrors everyone". Out of the box nothing propagates — sync only activates
once you set `sync_targets` on at least one clock.

### What propagates

- **Run-state:** `start` / `pause` / `reset` propagate the action. A `start`
  also carries the duration (the only duration-bearing packet) — so a bare
  duration edit on one clock never moves a peer's countdown.
- **Config:** travels **only** bundled inside a `start`, as the leader's
  effective settings for that run. The follower applies it **one-shot** and
  reverts to its own saved settings when its timer returns to Idle — a peer's
  `start` can never overwrite a clock's saved configuration.
- **Never propagated:** `sync_follow` / `sync_targets` themselves (each
  clock's own identity) and the live remaining seconds.

### Behavior notes

- Targeting is by the stable `uniqueID` (shown in the web UI); `all` spares
  you from listing ids.
- One-hop: an applied inbound command is never re-broadcast, so partial
  target lists do not chain.
- Delivery is best-effort: each packet is sent 3× and receivers de-duplicate,
  so a single dropped frame rarely desyncs a clock.
- Clocks announce themselves with a presence beacon every 30 s; that is what
  populates the HA Targets select's dynamic peer list.
- A clock with `show_timer=false` ignores inbound sync.

Set the two keys via `POST /api/timer` / `{prefix}/timer`
(`sync_follow`, `sync_targets`), via `dev.json`
(`timer_sync_follow`, `timer_sync_targets`), or via the two HA entities.

## Settings and persistence

All timer settings persist across reboots. The behaviour-tuning knobs
(`max_duration`, `remaining_publish_interval`, `finished_hold`,
`realert_interval`, `countdown_seconds`, `melody_tick`, `melody_end`,
`bar_enabled`, `icon_enabled`, `bar_color`, `bar_bg_color`, `sync_follow`,
`sync_targets`) are settable over HTTP/MQTT ([reference](api.md#timer-control))
and readable back via `GET /api/timer` and the HA attributes; a subset is also
editable in the on-device `TIMER` menu. `dev.json` can override any of them at
boot ([reference](dev.md)). Per-state icons (`icon_idle` / `icon_running` /
`icon_paused` / `icon_finished`) resolve against `/ICONS/<name>.{jpg,gif}`;
the two melodies against `/MELODIES/<name>.txt`, with built-in default tunes
when the files are absent.

## Build-time opt-out

The Timer feature is included by default. To build a firmware without it —
reclaiming roughly 29 kB of flash and 3 kB of RAM on space-constrained
devices — add `-DAWTRIX_DISABLE_TIMER` to `build_flags` in `platformio.ini`,
or set `PLATFORMIO_BUILD_FLAGS=-DAWTRIX_DISABLE_TIMER` when invoking
`pio run`. A disabled build has no Timer app, no `TIMER` on-device menu
entry, no `/api/timer` HTTP endpoint, no `{prefix}/timer` MQTT topic, no
Home Assistant timer entities, and no multi-device sync. The runtime
`SHOW_TIMER` setting is only meaningful in default builds.

Note when switching an already-provisioned device from a timer-enabled build
to a disabled one: the disabled firmware also lacks the HA discovery cleanup
code, so timer entities registered by the previous build linger as
"unavailable" in Home Assistant until you remove them there (or clear the
retained `homeassistant/…` discovery topics on the broker).

# MQTT / HTTP API

## Table of Contents
  * [Overview](#overview)
  * [Status Retrieval](#status-retrieval)
  * [LiveView](#liveview)
  * [Power Control](#power-control)
  * [Sound Playback](#sound-playback)
  * [Mood Lighting](#mood-lighting)
  * [Colored Indicators](#colored-indicators)
  * [Custom Apps and Notifications](#custom-apps-and-notifications)
    + [Interaction](#interaction)
    + [JSON Properties](#json-properties)
      - [Example](#example)
    + [Drawing Instructions](#drawing-instructions)
    + [Example](#example-1)
    + [Display Text in Colored Fragments](#display-text-in-colored-fragments)
    + [Sending Multiple Custom Pages Simultaneously](#sending-multiple-custom-pages-simultaneously)
    + [Delete a Custom App](#delete-a-custom-app)
    + [Dismiss Notification](#dismiss-notification)
    + [Switch Apps](#switch-apps)
    + [Switch to Specific App](#switch-to-specific-app)
  * [Timer Control](#timer-control)
  * [Change Settings](#change-settings)
    + [JSON Properties](#json-properties-1)
  * [Update](#update)
      - [Reboot Awtrix](#reboot-awtrix)
      - [Erase Awtrix](#erase-awtrix)
      - [Clear Settings](#clear-settings)


## Overview

This API documentation covers various functionalities such as retrieving device statistics, screen mirroring, notifications, custom apps, sound playing, and mood lighting. You can interact with these features via both MQTT and HTTP protocols.

## Status Retrieval

Access various device statistics like battery, RAM, and more:

| MQTT Topic                      | HTTP URL                           | Description                                    |
| ------------------------------- | ---------------------------------- | ---------------------------------------------- |
| `[PREFIX]/stats`                | `http://[IP]/api/stats`            | General device stats (e.g., battery, RAM)      |
| `[PREFIX]/stats/effects`        | `http://[IP]/api/effects`          | List of all effects                            |
| `[PREFIX]/stats/transitions`    | `http://[IP]/api/transitions`      | List of all transition effects                 |
| `[PREFIX]/stats/loop`           | `http://[IP]/api/loop`             | List of all apps in the loop                   |

> **Note:** MQTT also broadcasts other data, such as button presses and the current app.

## LiveView

Retrieve the current matrix screen as an array of 24-bit colors:

| MQTT Topic              | HTTP URL                     | Payload/Body | HTTP Method |
| ----------------------- | ---------------------------- | ------------ | ----------- |
| `[PREFIX]/sendscreen`   | `http://[IP]/api/screen`     | -            | GET         |

When triggering the MQTT API, AWTRIX sends the array to `[PREFIX]/screen`.

**Extras:**
- Access a live view of the screen in your browser: `http://[IP]/screen`.
- Options to download a screenshot or generate a GIF from the current display content.
- `http://[IP]/fullscreen` gives you a fullscreen liveview. Here you can optionally set the `fps` as parameter (standard 30).

## Power Control

Toggle the matrix on or off:

| MQTT Topic       | HTTP URL                      | Payload/Body              | HTTP Method |
| ---------------- | ----------------------------- | ------------------------- | ----------- |
| `[PREFIX]/power` | `http://[IP]/api/power`       | `{"power": true}` or `{"power": false}` | POST        |

Send the board in deep sleep mode (turns off the matrix as well), good for saving battery life:

| MQTT Topic       | HTTP URL                      | Payload/Body              | HTTP Method |
| ---------------- | ----------------------------- | ------------------------- | ----------- |
| `[PREFIX]/sleep` | `http://[IP]/api/sleep`       | `{"sleep": X}` where X is number of seconds | POST        |

AWTRIX will only wakeup after time or if you press the middle button once. There is no way to wake up via API.

## Sound Playback

Play a RTTTL sound from the MELODIES folder.
If you're using a DFplayer, use the /sound API and enter the 4 digit number of your MP3.

| MQTT Topic        | HTTP URL                     | Payload/Body        | HTTP Method |
| ----------------- | ---------------------------- | ------------------- | ----------- |
| `[PREFIX]/sound`  | `http://[IP]/api/sound`      | `{"sound":"alarm"}` | POST        |

Play a RTTTL sound from a given RTTTL string:

| MQTT Topic        | HTTP URL                     | Payload/Body  | HTTP Method |
| ----------------- | ---------------------------- | ------------- | ----------- |
| `[PREFIX]/rtttl`  | `http://[IP]/api/rtttl`      | `rttl string` | POST        |


## Mood Lighting

Set the entire matrix to a custom color or temperature:

| MQTT Topic             | HTTP URL                          | Payload/Body | HTTP Method |
| ---------------------- | --------------------------------- | ------------ | ----------- |
| `[PREFIX]/moodlight`   | `http://[IP]/api/moodlight`       | See below    | POST        |

> ⚠️ **Caution:** Using this function results in a higher current draw and heat, especially when all pixels are lit. Ensure you manage brightness values responsibly.

To disable moodlight, send an empty payload.

**Example:**
```json
{
  "brightness": 170,
  "kelvin": 2300
}
```

Possible moodlight options:
```json
{"brightness":170,"kelvin":2300}
or
{"brightness":170,"color":[155,38,182]}
or
{"brightness":170,"color":"#FF00FF"}
```


## Colored Indicators

Colored indicators serve as small notification signs displayed on specific areas of the screen:

- Upper right corner: Indicator 1
- Right side: Indicator 2
- Lower right corner: Indicator 3

| MQTT Topic             | HTTP URL                           | Payload/Body          | HTTP Method |
| ---------------------- | ---------------------------------- | --------------------- | ----------- |
| `[PREFIX]/indicator1`  | `http://[IP]/api/indicator1`       | `{"color":[255,0,0]}` | POST        |
| `[PREFIX]/indicator2`  | `http://[IP]/api/indicator2`       | `{"color":[0,255,0]}` | POST        |
| `[PREFIX]/indicator3`  | `http://[IP]/api/indicator3`       | `{"color":[0,255,0]}` | POST        |

**Color Options:**
- Use an RGB array, e.g., `{"color":[255,0,0]}`
- Use HEX color strings, e.g., `{"color":"#32a852"}`

**Hide Indicators:**
- To hide the indicators, send the color black (`{"color":[0,0,0]}`) or use the shorthand `{"color":"0"}`. Alternatively, send an empty payload.

**Additional Effects:**
- **Blinking**: To make the indicator blink, add the key `"blink"` with a value specifying the blinking interval in milliseconds.
- **Fading**: To make the indicator fade on and off, add the key `"fade"` with a value specifying the fade interval in milliseconds.

## Custom Apps and Notifications

With AWTRIX 3, you can design custom apps or notifications to showcase your unique text and icons.

### Interaction

- **MQTT**: Send a JSON object to `[PREFIX]/custom/[app]`, where `[app]` denotes your app's name (excluding spaces).
- **HTTP API**: Incorporate the app name in the query parameter (`name=[appname]`).
- **Updating**: To refresh a custom page, dispatch a modified JSON object to the identical endpoint. The display updates instantly.
- **One-Time Notification**: Use the same JSON format. Direct your JSON object to `[PREFIX]/notify` or `http://[IP]/api/notify`.

| MQTT Topic                  | HTTP URL                          | Payload/Body | Query Parameters | HTTP Method |
| --------------------------- | --------------------------------- | ------------ | ---------------- | ----------- |
| `[PREFIX]/custom/[appname]` | `http://[IP]/api/custom`          | JSON         | name=[appname]   | POST        |
| `[PREFIX]/notify`           | `http://[IP]/api/notify`          | JSON         | -                | POST        |

### JSON Properties

Below are the properties you can utilize in the JSON object. **All keys are optional**; only include the properties you require.

| Key | Type | Description | Default | Custom App | Notification |
| --- | ---- | ----------- | ------- | ------- | ------- |
| `text` | string | The text to display. Keep in mind the font does not have a fixed size and `I` uses less space than `W`. This affects when text will start scrolling. | N/A | X | X |
| `textCase` | integer | Changes the Uppercase setting. 0=global setting, 1 = forces uppercase; 2 = shows as sent. | 0 | X | X |
| `topText` | boolean | Draw the text on top. | false | X | X |
| `textOffset` | integer | Sets an offset for the x position of a starting text. | 0 | X | X |
| `center` | boolean | Centers a short, non-scrollable text. | true | X | X |
| `color` | string or array of integers | The text, bar or line color. | N/A | X | X |
| `gradient` | Array of string or integers | Colorizes the text in a gradient of two given colors. | N/A | X | X |
| `blinkText` | Integer | Blinks the text in an given interval in ms, not compatible with gradient or rainbow. | N/A | X | X |
| `fadeText` | Integer | Fades the text on and off in an given interval, not compatible with gradient or rainbow. | N/A | X | X |
| `background` | string or array of integers | Sets a background color. | N/A | X | X |
| `rainbow` | boolean | Fades each letter in the text differently through the entire RGB spectrum. | false | X | X |
| `icon` | string | The icon ID or filename (without extension) to display on the app. You can also send a **8x8 jpg** as Base64 string. | N/A | X | X |
| `pushIcon` | integer | 0 = Icon doesn't move. 1 = Icon moves with text and will not appear again. 2 = Icon moves with text but appears again when the text starts to scroll again. | 0 | X | X |
| `repeat` | integer | Sets how many times the text should be scrolled through the matrix before the app ends. | -1 | X | X |
| `duration` | integer | Sets how long the app or notification should be displayed. | 5 | X | X |
| `hold` | boolean | Set it to true, to hold your **notification** on top until you press the middle button or dismiss it via HomeAssistant. This key only belongs to notification. | false |  | X |
| `sound` | string | The filename of your RTTTL ringtone file placed in the MELODIES folder (without extension). Or the 4 digit number of your MP3 if you're using a DFplayer. | N/A |  | X |
| `rtttl` | string | Allows to send the RTTTL sound string with JSON. | N/A |  | X |
| `loopSound` | boolean | Loops the sound or RTTTL as long as the notification is running. | false |  | X |
| `bar` | array of integers | Draws a bargraph. Without icon maximum 16 values, with icon 11 values. | N/A | X | X |
| `line` | array of integers | Draws a linechart. Without icon maximum 16 values, with icon 11 values. | N/A | X | X |
| `autoscale` | boolean | Enables or disables autoscaling for bar and linechart. | true | X | X |
| `barBC` | string or array of integers | Backgroundcolor of the bars. | 0 | X | X |
| `progress` | integer | Shows a progress bar. Value can be 0–100. | -1 | X | X |
| `progressC` | string or array of integers | The color of the progress bar. | -1 | X | X |
| `progressBC` | string or array of integers | The color of the progress bar background. | -1 | X | X |
| `pos` | integer | Defines the position of your custom page in the loop, starting at 0 for the first position. This will only apply with your first push. This function is experimental. | N/A | X |  |
| `draw` | array of objects | Array of drawing instructions. Each object represents a drawing command. See the drawing instructions below. |  | X | X |
| `lifetime` | integer | Removes the custom app when there is no update after the given time in seconds. | 0 | X |  |
| `lifetimeMode` | integer | 0 = deletes the app, 1 = marks it as staled with a red rectangle around the app. | 0 | X |  |
| `stack` | boolean | Defines if the **notification** will be stacked. `false` will immediately replace the current notification. | true |  | X |
| `wakeup` | boolean | If the Matrix is off, the notification will wake it up for the time of the notification. | false |  | X |
| `noScroll` | boolean | Disables the text scrolling. | false | X | X |
| `clients` | array of strings | Allows forwarding a notification to other AWTRIX devices. Use the MQTT prefix for MQTT and IP addresses for HTTP. |  |  | X |
| `scrollSpeed` | integer | Modifies the scroll speed. Enter a percentage value of the original scroll speed. | 100 | X | X |
| `effect` | string | Shows an [effect](https://blueforcer.github.io/awtrix3/#/effects) as background. The effect can be removed by sending an empty string for effect. |  | X | X |
| `effectSettings` | json map | Changes color and speed of the [effect](https://blueforcer.github.io/awtrix3/#/effects). |  | X | X |
| `save` | boolean | Saves your custom app into flash and reloads it after boot. Avoid this for custom apps with high update frequencies because the ESP's flash memory has limited write cycles. |  | X |  |
| `overlay`| string  | Sets an effect overlay (cannot be used with global overlays). |  | X | X |

**Color**: Accepts a hex string or an R,G,B array: `"#FFFFFF"` or `[255,255,0]`.

**Overlay effects:**
- "clear"
- "snow"
- "rain"
- "drizzle"
- "storm"
- "thunder"
- "frost"


#### Example

Here's a sample JSON to present the text "Hello, AWTRIX 3!" in rainbow colors for a duration of 10 seconds:

```json
{
  "text": "Hello, AWTRIX 3!",
  "rainbow": true,
  "duration": 10
}
```

### MQTT Placeholder

This feature is particularly useful for users without a full smart home system. It eliminates the need for an external system to display data, such as from an inverter which can send its data via MQTT. You can simply create a [AppName].json file in the CUSTOMAPP folder with your custom app JSON keys. This JSON file will be loaded upon boot, so you don't need to send it from an external source. Or you can also use it in your HTTP or MQTT API request.

The placeholders inside the `text` value enclosed in `{{}}` will be replaced with the payload of the specified MQTT topic. Currently, there are no options available for formatting the payload.

```json
{"text": "Solar: {{inverter/total/P_AC}} W"}
```

### Drawing Instructions

!> Please note: Depending on the number of objects, the RAM usage can be very high. This could cause freezes or reboots.
It's important to be mindful of the number of objects and the complexity of the drawing instructions to avoid performance issues.

Each drawing instruction is an object with a required command key and an array of values depending on the command:

| Command | Array Values         | Description |
| ------- | -------------------- | ----------- |
| `dp`    | `[x, y, cl]`         | Draw a pixel at position (`x`, `y`) with color `cl` |
| `dl`    | `[x0, y0, x1, y1, cl]` | Draw a line from (`x0`, `y0`) to (`x1`, `y1`) with color `cl` |
| `dr`    | `[x, y, w, h, cl]`   | Draw a rectangle with top-left corner at (`x`, `y`), width `w`, height `h`, and color `cl` |
| `df`    | `[x, y, w, h, cl]`   | Draw a filled rectangle with top-left corner at (`x`, `y`), width `w`, height `h`, and color `cl` |
| `dc`    | `[x, y, r, cl]`      | Draw a circle with center at (`x`, `y`), radius `r`, and color `cl` |
| `dfc`   | `[x, y, r, cl]`      | Draw a filled circle with center at (`x`, `y`), radius `r`, and color `cl` |
| `dt`    | `[x, y, t, cl]`      | Draw text `t` with top-left corner at (`x`, `y`) and color `cl` |
| `db`    | `[x, y, w, h, [bmp]]`    | Draws a RGB888 bitmap array `[bmp]` with top-left corner at (`x`, `y`) and size of (`w`, `h`) |


### Example

Here's an example JSON object to draw a red circle, a blue rectangle, and the text "Hello" in green:

```json
{"draw":[
 {"dc": [28, 4, 3, "#FF0000"]},
 {"dr": [20, 4, 4, 4, "#0000FF"]},
 {"dt": [0, 0, "Hello", "#00FF00"]}
]}
```

### Display Text in Colored Fragments

AWTRIX 3 allows you to present text where specific fragments can be colorized. Use an array of fragments with `"t"` representing the text fragment and `"c"` denoting the color's hex value.

```json
{
  "text": [
    {
      "t": "Hello, ",
      "c": "FF0000"
    },
    {
      "t": "world!",
      "c": "00FF00"
    }
  ],
  "repeat": 2
}
```

### Sending Multiple Custom Apps Simultaneously

AWTRIX 3 enables you to dispatch multiple custom apps in a single action. Instead of transmitting one custom app object, you can forward an array of objects.

**e.g. MQTT Topic:** `/custom/test`

```json
[
  {"text":"1"},
  {"text":"2"}
]
```

**Handling of Multiple Custom Apps:**
- **Suffix Assignment**: Internally, the app name receives a suffix, turning it into a format like `test0`, `test1`, etc.
- **Updates**: You can refresh each app individually or update all of them collectively.
- **Deletion**:
  - When erasing apps, AWTRIX doesn't match the exact app name. Instead, it identifies apps that begin with the specified name.
  - To expunge all associated apps, send an empty payload to `/custom/test`. This action will remove `test0`, `test1`, and so on.
  - To eradicate a single app, direct the command to, for instance, `/custom/test1`.
  - Caution: Deleting just one app may upset the sequence of the remaining apps in the loop, as there's no provision for placeholders to retain order.




### Delete a Custom App

To remove a custom app, dispatch an empty payload/body to the associated topic or URL.

### Dismiss Notification

Easily dismiss a notification that was configured with `"hold": true`.

| MQTT Topic                   | HTTP URL                           | Payload/Body       | HTTP Method |
| ---------------------------- | ---------------------------------- | ------------------ | ----------- |
| `[PREFIX]/notify/dismiss`    | `http://[IP]/api/notify/dismiss`   | Empty payload/body | POST        |

### Switch Apps

Navigate to the next or preceding app.

| MQTT Topic                   | HTTP URL                           | Payload/Body       | HTTP Method |
| ---------------------------- | ---------------------------------- | ------------------ | ----------- |
| `[PREFIX]/nextapp`           | `http://[IP]/api/nextapp`          | Empty payload/body | POST        |
| `[PREFIX]/previousapp`       | `http://[IP]/api/previousapp`      | Empty payload/body | POST        |

### Switch to Specific App

Directly transition to a desired app using its name.

| MQTT Topic                   | HTTP URL                           | Payload/Body      | HTTP Method |
| ---------------------------- | ---------------------------------- | ----------------- | ----------- |
| `[PREFIX]/switch`            | `http://[IP]/api/switch`           | `{"name":"Time"}` | POST        |

**Built-in App Names**:
- `Time`
- `Date`
- `Temperature`
- `Humidity`
- `Battery`
- `Timer`

For custom apps, employ the name you designated in the topic or HTTP parameter. In MQTT, if `[PREFIX]/custom/test` is your topic, then `test` would be the app's name.


## Timer Control

Control the built-in Timer app. See the [Timer app overview](https://blueforcer.github.io/awtrix3/#/apps?id=timer) for behaviour details.

| MQTT Topic       | HTTP URL                  | Payload/Body | HTTP Method |
| ---------------- | ------------------------- | ------------ | ----------- |
| `[PREFIX]/timer` | `http://[IP]/api/timer`   | JSON (see below) | POST    |
| –                | `http://[IP]/api/timer`   | – (returns JSON snapshot, see [State observation](#state-observation)) | GET |

All JSON properties are optional. When multiple are sent together, property setters apply first and then `action` runs — so `{"duration":600,"action":"start"}` starts a fresh 10-minute timer in one request.

#### JSON Properties

| Key | Type | Values | Description |
| --- | --- | --- | --- |
| `action` | string | `start` / `pause` / `reset` | Performs the action. `start` from `idle` also force-switches the display to the Timer app (gated on no active game). `start` is a no-op while already running. `start` from `finished` restarts the countdown. |
| `duration` | string or integer | A clock string `"HH:MM:SS"`/`"MM:SS"`, or a bare integer of seconds. Must be 1 .. `timer_max_duration`. | Sets the configured duration. Colon count decides units (`"3:00"` = 3 min, never 3 h); out-of-range fields carry (`"3:90"` = 270 s). A malformed string **or** an out-of-range value is **rejected** (HTTP `400`, see Responses), not clamped. Published back as a trimmed clock string (`300` → `5:00`). Persists across reboots. Mid-run writes buffer until next `start`/`reset`. |
| `buzzer` | string | `off` / `end` / `countdown` | Sets buzzer mode. Persists. |
| `finished` | string | `auto-clear` / `hold` / `re-alert` | Sets what happens at `00:00`. Persists. |
| `icon_idle` | string | Bare icon name resolved against `/ICONS/<name>.{jpg,gif}`; empty clears the slot; capped at 32 chars | Icon shown in the **Idle** state and used as fallback for any other state whose slot is empty. Persists. |
| `icon_running` | string | Same. Empty clears (then falls back to `icon_idle`). | Icon shown while counting down. Persists. |
| `icon_paused` | string | Same. Empty clears (then falls back to `icon_idle`). | Icon shown while paused. Persists. |
| `icon_finished` | string | Same. Empty clears (then falls back to `icon_idle`). | Icon shown beneath the blinking `0:00`. Persists. |
| `finished_hold`     | integer | 1–300 (s)            | Auto-clear delay (only meaningful when `finished = "auto-clear"`). Persists. |
| `realert_interval`  | integer | 5–300 (s)            | Re-alert cadence (only meaningful when `finished = "re-alert"`). Persists. |
| `countdown_seconds` | integer | 0–30   (s)           | Pre-expiry beep window (only meaningful when `buzzer = "countdown"`). Persists. |
| `max_duration`               | integer | 1–604800 (s, 1 s .. 7 days) | Upper bound on accepted `duration`. Persists. |
| `remaining_publish_interval` | integer | 1–60 (s)             | How often `timer_rem` republishes while Running. Persists. |
| `melody_tick` | string | **Either** a bare name resolved against `/MELODIES/<name>.txt` (≤32 chars) **or** an inline RTTTL tune | RTTTL countdown-beep melody. A bare name persists and obeys `save`; an **inline tune is always one-shot** (never saved). |
| `melody_end`  | string | Same. A bare empty string resets to default `"timer_end"`. | RTTTL end melody. Bare name persists & obeys `save`; inline tune always one-shot. |
| `bar_enabled` | bool | `true` / `false` | Show/hide the Running/Paused progress bar. Persists. |
| `icon_enabled` | bool | `true` / `false` | Show/hide the timer icon; when hidden the time text and bar reflow to span the full panel. Persists. |
| `bar_color`   | int or hex string | `0..0xFFFFFF` or `"#RRGGBB"` / `"RRGGBB"` | Progress-bar (foreground) color; `0` follows `TEXTCOLOR_888`. Persists. |
| `bar_bg_color` | int or hex string | `0..0xFFFFFF` or `"#RRGGBB"` / `"RRGGBB"` | Progress-bar **background track** color (drawn behind the bar); `0` = black = no track (the default — bar looks unchanged). Unlike `bar_color`'s `0`, this is a literal black, not a sentinel. Persists. |
| `sync_follow`  | bool | `true` / `false` | Obey inbound multi-device timer-sync this clock is targeted by (follow consent gate). Local identity — not propagated. Persists. |
| `sync_targets` | string | `""` / `all` / comma list of peer `uniqueID`s | Whom this clock commands on a local timer action. Local identity — not propagated. Persists. |
| `save` | bool | `true` (default) / `false` | `save:false` makes the **whole command one-shot**: its config applies to the current run only and reverts to the saved settings when the timer next returns to Idle (a `reset` or auto-clear), writing nothing to flash. A non-boolean is rejected (atomic-reject). See the one-shot note below. |

`action` / `buzzer` / `finished` values are case-insensitive; `auto-clear`/`autoclear` and `re-alert`/`realert` are both accepted. Icon and **bare** melody values are **case-sensitive** (they map to filenames on LittleFS) and **capped at 32 characters** (alphanumeric, `_`, `-`). The icon loader checks `/ICONS/<name>.jpg` then `/ICONS/<name>.gif`; the melody loader reads `/MELODIES/<name>.txt`.

**One-shot commands (`save:false`) and inline melodies**. By default every config key persists to flash and becomes your new default. Send `save:false` to run a timer **once** with custom parameters: its config applies to the current run only and reverts when the timer next returns to Idle, writing nothing to flash. While a one-shot run is active the **observation surface stays honest** — the `GET /api/timer` `config` mirror, the Home Assistant attribute entities, and device-to-device sync all keep reporting your **saved** configuration, never the one-off values (the top-level `duration`/`buzzer`/`finished` still show the live values). `melody_end`/`melody_tick` additionally accept an **inline RTTTL tune** (a literal melody string such as `"alarm:d=4,o=5,b=120:c,e,g"`, detected by its `:`-separated `d=`/`o=`/`b=` structure; a malformed tune is rejected `400`); an inline tune is **always one-shot** regardless of `save`, never persists, and **never appears** in the `config` mirror (which shows the saved bare name throughout). The on-device TIMER menu always persists.

#### Responses

`POST /api/timer` validates the whole command **atomically** — if any field is invalid, **nothing is applied**:

| Status | Body | When |
| --- | --- | --- |
| `200 OK` | `OK` | Command accepted and applied. |
| `400 Bad Request` | `ErrorParsingJson` | Body isn't valid JSON (or exceeds the parse buffer). |
| `400 Bad Request` | `InvalidValue` | A field has an unusable value: malformed/out-of-range `duration`, or an unknown `buzzer`/`finished`/`action`/icon. |
| `409 Conflict` | `TimerDisabled` | The Timer feature is off (`SHOW_TIMER=false`); the command is understood but can't apply. |

Out-of-range durations are **rejected**, not clamped (e.g. `"30:00:00"` when the max is 24 h → `400`). The `{prefix}/timer` MQTT topic applies the same validation but, being fire-and-forget, ignores invalid commands silently (no error is published); the retained `timer_dur` state reflects the unchanged value.

#### Examples

Start a 5-minute timer immediately (numeric seconds or the equivalent clock string):
```json
{"duration": 300, "action": "start"}
```
```json
{"duration": "5:00", "action": "start"}
```

Change buzzer mode mid-run (takes effect immediately for the countdown ticks):
```json
{"buzzer": "countdown"}
```

Reset to idle (also clears the finished screen if visible):
```json
{"action": "reset"}
```

Set per-state icons (idle stays as fallback; Running uses an animated GIF):
```json
{"icon_idle": "64936", "icon_running": "74706"}
```

Clear an override so the slot falls back to the Idle icon:
```json
{"icon_paused": ""}
```

#### State observation

`GET http://[IP]/api/timer` returns a read-only JSON snapshot of the live timer. This is an **observation** endpoint — it never mutates state and takes no body. It always responds `200 OK`; there is no `409` when the timer is disabled (the `enabled` field carries that bit instead).

```json
{
  "state": "running",
  "enabled": true,
  "remaining": 174,
  "remaining_str": "2:54",
  "duration": 300,
  "duration_str": "5:00",
  "buzzer": "end",
  "finished": "auto-clear",
  "config": {
    "finished_hold": 10,
    "realert_interval": 15,
    "countdown_seconds": 3,
    "max_duration": 86400,
    "max_duration_str": "24:00:00",
    "remaining_publish_interval": 1,
    "icon_enabled": true,
    "bar_enabled": true,
    "bar_color": "default",
    "bar_bg_color": "none",
    "melody_tick": "timer_tick",
    "melody_end": "timer_end",
    "sync_follow": true,
    "sync_targets": "",
    "buzzer": "end",
    "finished": "auto-clear",
    "icon_idle": "",
    "icon_running": "",
    "icon_paused": "",
    "icon_finished": ""
  }
}
```

The response has two parts: the stable top-level **run-state summary** (the eight fields below, byte-compatible with earlier firmware) and a nested **`config`** object mirroring the full persisted configuration.

| Field | Type | Description |
| --- | --- | --- |
| `state` | string | `idle` / `running` / `paused` / `finished`. |
| `enabled` | bool | Mirrors the `TIMER` master enable (`SHOW_TIMER`). When `false` the timer is forced to `idle`; the snapshot is still returned. |
| `remaining` | integer | Seconds remaining, computed live at request time (wall-clock accurate). Frozen while `paused`; `0` while `finished`. |
| `remaining_str` | string | `remaining` as a trimmed clock string (same format as `duration_str`, e.g. `174` → `2:54`). |
| `duration` | integer | Configured duration in seconds. **Run-state, not config** — it stays top-level only and is deliberately absent from `config`. |
| `duration_str` | string | `duration` as a trimmed clock string (`300` → `5:00`, hours dropped when zero). |
| `buzzer` | string | Buzzer mode: `off` / `end` / `countdown`. Also mirrored inside `config`. |
| `finished` | string | Finished mode: `auto-clear` / `hold` / `re-alert`. Also mirrored inside `config`. |

The `buzzer` / `finished` strings are the **canonical output spellings** (hyphenated, lowercase). The `POST` command parser additionally tolerates aliases on input (e.g. `autoclear`, `realert`), but the read endpoint always reports the canonical form.

##### `config` — persisted configuration mirror

The `config` object reports **every persisted Timer configuration key** — exactly the keys you can write via `POST /api/timer` — so an HTTP-only integrator can read the device's live configuration (and confirm a write applied) without Home Assistant or an MQTT subscription. It is projected from the same descriptor tables that drive the control surface and the HA attribute groups, so the read surface cannot drift from what is writable: adding a new persisted config key automatically makes it readable here.

While a **one-shot** run (`save:false`, see above) is active, `config` reports your **saved** configuration, never the one-off values — so a read-after-write check sees the saved truth, not the transient run-only override. The top-level `duration`/`buzzer`/`finished` continue to show the live one-shot values. An inline RTTTL tune never appears here; `config.melody_*` shows the saved bare name throughout.

Values are **raw** by default (interval seconds as integers, melodies and `sync_targets` as strings, toggles as booleans), with two deliberate carrier-native renderings that follow this endpoint's own raw+`_str` duration precedent:

- **`bar_color`** is rendered as `"#RRGGBB"` (uppercase) or `"default"` when it follows the text color, rather than a raw 24-bit integer.
- **`bar_bg_color`** is rendered as `"#RRGGBB"` (uppercase) or `"none"` when it is black (no track), rather than a raw integer — the `"none"` vs `"default"` wording reflects the deliberate sentinel asymmetry between the two colors.
- **`max_duration`** is reported **both** raw (`86400`) **and** as `max_duration_str` (`"24:00:00"`, trimmed clock form).

| `config` key | Type | Notes |
| --- | --- | --- |
| `finished_hold` | integer | Seconds the finished screen holds (hold mode). |
| `realert_interval` | integer | Seconds between re-alerts (re-alert mode). |
| `countdown_seconds` | integer | Length of the pre-end countdown tick window. |
| `max_duration` | integer | Maximum settable duration, raw seconds. |
| `max_duration_str` | string | `max_duration` as a trimmed clock string (carrier-native). |
| `remaining_publish_interval` | integer | Seconds between `remaining` MQTT pushes. |
| `icon_enabled` | bool | Whether per-state icons are shown. |
| `bar_enabled` | bool | Whether the progress bar is shown. |
| `bar_color` | string | `"#RRGGBB"` or `"default"` (carrier-native; not the raw integer). |
| `bar_bg_color` | string | `"#RRGGBB"` or `"none"` (carrier-native; not the raw integer). Background track behind the bar. |
| `melody_tick` | string | RTTTL melody name for countdown ticks. |
| `melody_end` | string | RTTTL melody name played at finish. |
| `sync_follow` | bool | Whether this clock follows synced peers (observable only; never HA-writable or propagated). |
| `sync_targets` | string | Multi-device sync targets: `""` (off), `all`, or a comma list of device IDs (observable only). |
| `buzzer` | string | Buzzer mode — mirrored here for completeness (also top-level). |
| `finished` | string | Finished mode — mirrored here for completeness (also top-level). |
| `icon_idle` / `icon_running` / `icon_paused` / `icon_finished` | string | The four per-state icon names (empty string = falls back to the Idle icon). |

`config` never contains `duration` (run-state, reported top-level only) or `action` (a transient command verb, not persisted). The endpoint stays a pure observation read regardless: always `200 OK`, never mutating, no `409`.

> **Note on freshness vs. Home Assistant:** `remaining` here is computed at the moment of the request, so during `running` it can read a second or two lower than the HA `{id}_timer_rem` sensor, which is push-throttled to `remaining_publish_interval` (default 1 s). This is expected — the polled HTTP read is simply fresher than the throttled push sensor; the two are not in disagreement.

When `HA_DISCOVERY` is enabled, Home Assistant also receives live updates of all timer properties via MQTT discovery sensors — an alternative path for observing the timer remotely.

The current icon configuration is also published as a retained JSON message to `[PREFIX]/timer/icons` whenever it changes (and on MQTT connect). Subscribe to that topic to read back current icon slots without HA. Payload shape: `{"idle": "...", "running": "...", "paused": "...", "finished": "..."}`. The same four icon names are now also readable over HTTP under `config.icon_*` (above) — Home Assistant deliberately has no icon read path, since it cannot usefully render an AWTRIX icon file.


## Change Settings

Adjust various settings related to the app display.

| MQTT Topic            | HTTP URL                          | Payload/Body  | HTTP Method |
| --------------------- | --------------------------------- | ------------- | ----------- |
| `[PREFIX]/settings`   | `http://[IP]/api/settings`        | JSON          | GET/POST        |

### JSON Properties

You can adjust each property in the JSON object according to your preferences. Including a property is optional.

| Key           | Type                      | Description                                                                                         | Value Range                                        | Default |
| ------------- | ------------------------- | --------------------------------------------------------------------------------------------------- | -------------------------------------------------- | ------- |
| `ATIME`       | number                    | Duration an app is displayed in seconds.                                                            | Positive integer                                   | 7       |
| `TEFF`        | number                    | Choose between app transition effects.                                                              | 0–10                                               | 1       |
| `TSPEED`      | number                    | Time taken for the transition to the next app in milliseconds.                                      | Positive integer                                   | 500     |
| `TCOL`        | string/array of ints      | Global text color.                                                                                  | RGB array or hex color                             | N/A     |
| `TMODE`       | integer                   | Changes the time app style.                                                                         | 0–6                                                | 1       |
| `CHCOL`       | string/array of ints      | Calendar header color of the time app.                                                              | RGB array or hex color                             |`#FF0000`|
| `CBCOL`       | string/array of ints      | Calendar body color of the time app.                                                                | RGB array or hex color                             |`#FFFFFF`|
| `CTCOL`       | string/array of ints      | Calendar text color in the time app.                                                                | RGB array or hex color                             |`#000000` |
| `WD`          | boolean                   | Enable or disable the weekday display.                                                              | `true`/`false`                                     | true    |
| `WDCA`        | string/array of ints      | Active weekday color.                                                                               | RGB array or hex color                             | N/A     |
| `WDCI`        | string/array of ints      | Inactive weekday color.                                                                             | RGB array or hex color                             | N/A     |
| `BRI`         | number                    | Matrix brightness.                                                                                  | 0–255                                              | N/A     |
| `ABRI`        | boolean                   | Automatic brightness control.                                                                       | `true`/`false`                                     | N/A     |
| `ATRANS`      | boolean                   | Automatic switching to the next app.                                                                | `true`/`false`                                     | N/A     |
| `CCORRECTION` | array of ints             | Color correction for the matrix.                                                                    | RGB array                                          | N/A     |
| `CTEMP`       | array of ints             | Color temperature for the matrix.                                                                   | RGB array                                          | N/A     |
| `TFORMAT`     | string                    | Time format for the TimeApp.                                                                        | Varies (see below)                                 | N/A     |
| `DFORMAT`     | string                    | Date format for the DateApp.                                                                        | Varies (see below)                                 | N/A     |
| `SOM`         | boolean                   | Start the week on Monday.                                                                           | `true`/`false`                                     | true    |
| `CEL`         | boolean                   | Shows the temperature in Celsius (Fahrenheit when false).                                           | `true`/`false`                                     | true    |
| `BLOCKN`      | boolean                   | Block physical navigation keys (still sends input to MQTT).                                         | `true`/`false`                                     | false   |
| `UPPERCASE`   | boolean                   | Display text in uppercase.                                                                          | `true`/`false`                                     | true    |
| `TIME_COL`    | string/array of ints      | Text color of the time app. Use 0 for global text color.                                            | RGB array or hex color                             | N/A     |
| `DATE_COL`    | string/array of ints      | Text color of the date app. Use 0 for global text color.                                            | RGB array or hex color                             | N/A     |
| `TEMP_COL`    | string/array of ints      | Text color of the temperature app. Use 0 for global text color.                                     | RGB array or hex color                             | N/A     |
| `HUM_COL`     | string/array of ints      | Text color of the humidity app. Use 0 for global text color.                                        | RGB array or hex color                             | N/A     |
| `BAT_COL`     | string/array of ints      | Text color of the battery app. Use 0 for global text color.                                         | RGB array or hex color                             | N/A     |
| `SSPEED`      | integer                   | Scroll speed modification.                                                                          | Percentage of original scroll speed                | 100     |
| `TIM`         | boolean                   | Enable or disable the native time app. App rotation refreshes immediately.                          | `true`/`false`                                     | true    |
| `DAT`         | boolean                   | Enable or disable the native date app. App rotation refreshes immediately.                          | `true`/`false`                                     | true    |
| `HUM`         | boolean                   | Enable or disable the native humidity app. App rotation refreshes immediately.                      | `true`/`false`                                     | true    |
| `TEMP`        | boolean                   | Enable or disable the native temperature app. App rotation refreshes immediately.                   | `true`/`false`                                     | true    |
| `BAT`         | boolean                   | Enable or disable the native battery app. App rotation refreshes immediately.                       | `true`/`false`                                     | true    |
| `TIMER`       | boolean                   | Master enable for the Timer app. When `false`: app hidden, HA entities suppressed, `/api/timer` and `{prefix}/timer` ignored, running timer reset. HA entity removal completes after next MQTT reconnect. | `true`/`false`                                     | true    |
| `MATP`        | boolean                   | Enable or disable the matrix. Similar to `power` endpoint but without the animation.                | `true`/`false`                                     | true    |
| `VOL`         | integer                   | Allows to set the volume of the buzzer and DFplayer.                                                 | 0–30                                               | true    |
| `OVERLAY`     | string                    | Sets a global effect overlay (cannot be used with app specific overlays).                            | Varies (see below)                                 | N/A     |

**Color Values**: Can either be an RGB array (e.g., `[255,0,0]`) or a valid 6-digit hexadecimal color value (e.g., `"#FF0000"` for red).

**Overlay effects:**
- "clear"
- "snow"
- "rain"
- "drizzle"
- "storm"
- "thunder"
- "frost"

#### **Available Time Formats:**
| Format       | Example    | Description                                |
|--------------|------------|--------------------------------------------|
| `%H:%M:%S`   | `13:30:45` | 24-hour time with seconds                  |
| `%l:%M:%S`   | `1:30:45`  | 12-hour time with seconds                  |
| `%H:%M`      | `13:30`    | 24-hour time                               |
| `%H %M`      | `13.30`    | 24-hour time with blinking colon           |
| `%l:%M`      | `1:30`     | 12-hour time                               |
| `%l %M`      | `1:30`     | 12-hour time with blinking colon           |
| `%l:%M %p`   | `1:30 PM`  | 12-hour time with AM/PM indicator          |
| `%l %M %p`   | `1:30 PM`  | 12-hour time with blinking colon and AM/PM |

#### **Available Date Formats:**
| Format       | Example    | Description            |
|--------------|------------|------------------------|
| `%d.%m.%y`   | `16.04.22` | Day.Month.Year (short) |
| `%d.%m`      | `16.04`    | Day.Month              |
| `%y-%m-%d`   | `22-04-16` | Year-Month-Day         |
| `%m-%d`      | `04-16`    | Month-Day              |
| `%m/%d/%y`   | `04/16/22` | Month/Day/Year         |
| `%m/%d`      | `04/16`    | Month/Day              |
| `%d/%m/%y`   | `16/04/22` | Day/Month/Year         |
| `%d/%m`      | `16/04`    | Day/Month              |
| `%m-%d-%y`   | `04-16-22` | Month-Day-Year         |

#### **Available Transition Effects:**
| Code | Effect      |
|------|-------------|
| `0`  | Random      |
| `1`  | Slide       |
| `2`  | Dim         |
| `3`  | Zoom        |
| `4`  | Rotate      |
| `5`  | Pixelate    |
| `6`  | Curtain     |
| `7`  | Ripple      |
| `8`  | Blink       |
| `9`  | Reload      |
| `10` | Fade        |

## Update

You can initiate the firmware update either through the update button in HA or using the following:

| MQTT Topic          | HTTP URL                          | Payload/Body | HTTP Header        | HTTP Method |
|---------------------|-----------------------------------|---------------|--------------------|-------------|
| `[PREFIX]/doupdate` | `http://[IP]/api/doupdate`        | JSON          | empty payload/body | POST        |

#### Reboot Awtrix
If you need to restart the Awtrix:

| MQTT Topic        | HTTP URL                     | Payload/Body | HTTP Method |
|-------------------|------------------------------|--------------|-------------|
| `[PREFIX]/reboot` | `http://[IP]/api/reboot`     | -            | POST        |

#### Erase Awtrix
**WARNING**: This action will format the flash memory and EEPROM but will not modify the WiFi Settings. It essentially serves as a factory reset.

| MQTT Topic      | HTTP URL                      | Payload/Body  | HTTP Method |
|-----------------|-------------------------------|---------------|-------------|
| `N/A`           | `http://[IP]/api/erase`       | -             | POST        |

#### Clear Settings
**WARNING**: This action will reset all settings from the settings API. It does not reset the flash files and WiFi Settings.

| MQTT Topic      | HTTP URL                        | Payload/Body  | HTTP Method |
|-----------------|---------------------------------|---------------|-------------|
| `N/A`           | `http://[IP]/api/resetSettings` | -             | POST        |

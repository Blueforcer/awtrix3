# Dev Features

This section contains small setting options that the majority of users do not change or change very rarely and therefore saved the effort of creating an elaborate settings interface.

All features are only applied at boot. So you have to restart awtrix after modifying.   

Create a `dev.json` in your filemanager.

## JSON Properties

The JSON object has the following properties:

| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `hostname` | string | Changes the hostname of your awtrix. This is used for App discovery, mDNS etc. | uniqeID |
| `ap_timeout` | integer | The timeout in seconds before AWTRIX switches to AP mode if the saved WLAN was not found. | 15 |
| `bootsound` | string | Uses a custom melodie while booting |  |
| `matrix` | integer | Changes the matrix layout (0,1 or 2) | `0` |
| `color_correction` | array of int | Sets the colorcorrection of the matrix | `[255,255,255]` |
| `color_temperature` | array of int | Sets the colortemperature of the matrix | `[255,255,255]` |
| `rotate_screen` | boolean | Rotates the screen upside down | `false` |
| `mirror_screen` | boolean | Mirrors the screen | `false` |
| `temp_dec_places` | integer | Number of decimal places for temperature measurements | `0` |
| `sensor_reading` | boolean | Enables or disables the reading of the Temp&Hum sensor | `true` |
| `temp_offset` | float | Sets the offset for the internal temperature measurement | `-9` |
| `hum_offset` | float | Sets the offset for the internal humidity measurement | `0` |
| `min_brightness` | integer | Sets minimum brightness level for the Autobrightness control | `2` |
| `max_brightness` | integer | Sets maximum brightness level for the Autobrightness control. On high levels, this could result in overheating! | `180` |
| `ldr_gamma` | float | Allows to set the gammacorrection of the brightness control | 3.0 |  
| `ldr_factor` | float | This factor is calculated into the raw ldr value wich is 0-1023 | 1.0 |  
| `min_battery` | integer | Calibrates the minimum battery measurement by the given raw value. You will get that from the stats api | `475` |
| `max_battery` | integer | Calibrates the maximum battery measurement by the given raw value. You will get that from the stats api | `665` |
| `ha_prefix` | string | Sets the prefix for Homassistant discovery | `homeassistant` |
| `background_effect` | string | Sets an [effect](https://blueforcer.github.io/awtrix3/#/effects) as global background layer | - |
| `stats_interval` | integer | Sets the interval in milliseconds when awtrix should send its stats to HA and MQTT | 10000 |
| `debug_mode` | boolean | Enables serial debug outputs. | false |
| `dfplayer` | boolean | Enables DFPLayer for Awtrix2_conversation builds. | false |
| `buzzer_volume` | boolean | Activates the volume control for the buzzer, doesnt work with every tones | false |
| `button_callback` | string | http callback url for button presses. | - |
| `new_year` | boolean | Displays fireworks and plays a jingle at newyear. | false |
| `swap_buttons` | boolean | Swaps the left and right hardware button. | false |
| `ldr_on_ground` | boolean | Sets the LDR configuration to LDR-on-ground. | false |
| `show_timer` | boolean | Master enable for the [Timer app](https://blueforcer.github.io/awtrix3/#/apps?id=timer). When `false`: app removed from rotation, the 10 Timer HA entities not published, `POST /api/timer` and the `{prefix}/timer` MQTT topic ignored, any running timer reset. On the `true → false` transition the firmware publishes empty retained discovery payloads so HA prunes the stale entities. Also exposed via `/api/settings` (`TIMER` key) and the on-device **APPS** menu. | `true` |
| `timer_max_duration` | integer | Upper bound on accepted `duration` values (range 1–604800). Out-of-range duration commands are rejected, not clamped. Also caps the HH/MM/SS wheels in the Timer-app config mode. Promoted to `{prefix}/timer` / `/api/timer`. | `86400` |
| `timer_remaining_publish_interval` | integer | Seconds between `timer_rem` republishes while Running (range 1–60). Drives the HA `{id}_timer_rem` sensor cadence. | `1` |
| `timer_finished_hold` | integer | Seconds the `00:00` Finished screen stays visible in Auto-clear finished mode (range 1–300). | `10` |
| `timer_realert_interval` | integer | Seconds between buzzer re-fires in Re-alert finished mode (range 5–300). | `15` |
| `timer_countdown_seconds` | integer | Tick window (seconds before zero) in Countdown buzzer mode (range 0–30). | `3` |
| `timer_icon_idle` | string | Bare icon name (resolved against `/ICONS/<name>.{jpg,gif}`) for the Timer app's **Idle** state. Also used as fallback for any other state with an empty slot. Empty/absent = no override of the NVS-stored value. Capped at 32 chars. **Overrides NVS on every boot.** | `""` |
| `timer_icon_running` | string | Same, for the Running state. Empty falls back to `timer_icon_idle`. | `""` |
| `timer_icon_paused` | string | Same, for the Paused state. Empty falls back to `timer_icon_idle`. | `""` |
| `timer_icon_finished` | string | Same, for the Finished (blinking `0:00`) state. Empty falls back to `timer_icon_idle`. | `""` |
| `timer_melody_tick` | string | Bare melody name (resolved against `/MELODIES/<name>.txt`) for countdown beeps. Empty resets to default `"timer_tick"`. | `"timer_tick"` |
| `timer_melody_end` | string | Bare melody name for the end melody. Empty resets to default `"timer_end"`. | `"timer_end"` |
| `timer_bar_enabled` | boolean | Show/hide the Running/Paused progress bar. | `true` |
| `timer_icon_enabled` | boolean | Show/hide the timer icon; when hidden the time text and bar reflow to the full panel. | `true` |
| `timer_bar_color` | integer | Hex color (0..0xFFFFFF) for the progress bar (foreground). `0` follows `TEXTCOLOR_888`. | `0` |
| `timer_bar_bg_color` | integer | Hex color (0..0xFFFFFF) for the progress-bar **background track** drawn behind the bar. `0` = black = no track (literal, not a sentinel). | `0` |
| `timer_sync_follow` | boolean | When `true`, this clock obeys inbound timer-sync packets it is targeted by (multi-device sync follow consent gate). Local identity — never propagated. | `true` |
| `timer_sync_targets` | string | Whom this clock commands on a local timer action: `""` (sync off), `all`, or a comma list of peer device IDs (e.g. `awtrix_ab12,awtrix_cd34`). Local identity — never propagated. | `""` |


#### Example:
```json
{
  "temp_dec_places":1,
  "bootsound":true,
  "hum_offset":-2
}
```
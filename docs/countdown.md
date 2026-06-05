# Countdown App

The Countdown app is a native app that shows how many calendar days remain until a configured target date.

The display layout is:

- Left: target day of month and the configured icon.
- Right: remaining days.

On the target date, the app hides the target day and remaining-day count, and only shows the configured icon. After the target date has passed, the Countdown app turns itself off until a future target date is configured and the app is enabled again.

## Enable Countdown

Set `COUNT` to `true` through the Settings API.

HTTP example:

```bash
curl -X POST "http://[IP]/api/settings" \
  -H "Content-Type: application/json" \
  -d '{"COUNT":true,"CDATE":"2026-06-01","CICON":"1234"}'
```

MQTT example:

```text
Topic: [PREFIX]/settings
Payload: {"COUNT":true,"CDATE":"2026-06-01","CICON":"1234"}
```

Replace:

- `[IP]` with the AWTRIX IP address.
- `[PREFIX]` with the configured MQTT prefix.
- `2026-06-01` with your target date.
- `1234` with the icon ID or filename from the existing icon manager.

## Set The Initial Target Date

The target date is configured with `CDATE`.

```json
{
  "CDATE": "2026-06-01"
}
```

`CDATE` must use `YYYY-MM-DD`.

The app counts full calendar days based on the device's local date:

- Today to tomorrow: `1`
- Today to today: only the configured icon is shown.
- Today to yesterday: Countdown automatically turns off.

## Set The Icon

The icon is configured with `CICON`.

```json
{
  "CICON": "1234"
}
```

`CICON` uses the existing icon management system. Use the icon ID or filename without extension. The firmware looks for:

- `/ICONS/1234.jpg`
- `/ICONS/1234.gif`

If the icon is missing before the target date, the Countdown app still shows the date and remaining days. On the target date, a missing icon leaves the Countdown screen blank.

## Optional Text Color

Set `COUNT_COL` to customize the countdown text color.

```json
{
  "COUNT_COL": "#00FF00"
}
```

Use `0` to fall back to the global text color.

## Disable Countdown

Set `COUNT` to `false`.

```bash
curl -X POST "http://[IP]/api/settings" \
  -H "Content-Type: application/json" \
  -d '{"COUNT":false}'
```

## Full Example

This enables Countdown, sets the initial target date, sets the icon, and sets the text color:

```json
{
  "COUNT": true,
  "CDATE": "2026-06-01",
  "CICON": "1234",
  "COUNT_COL": "#FFFFFF"
}
```

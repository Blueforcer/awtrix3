# Custom Pages & Notifications

With AWTRIX Light, you can create custom pages or notifications to display your own text and icons.
Simply send a JSON object to the topic "awtrixlight/custom/[page]" where [page] is a the name of your page (without spaces).

## JSON Properties

The JSON object has the following properties:

| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `text` | string | The text to display on the page. | |
| `icon` | string | The icon ID or filename (without extension) to display on the page. | |
| `repeat` | number | Sets how many times the text should be scrolled through the matrix before the display ends. | 1 |
| `rainbow` | boolean | Fades each letter in the text differently through the entire RGB spectrum. | false |
| `duration` | number | Sets how long the page should be displayed. | 5 |
| `color` | string | A color hex string for the text color. | "#FFFFFF" |
| `hold` | boolean | Set it to true, to hold your notification on top until you press the middle button or dismiss it via HomeAssistant. This key only belongs to notification. | false |
| `sound` | string | The filename of your RTTTL ringtone file (without extension). | |
| `pushIcon` | number | 0 = Icon doesn't move. 1 = Icon moves with text and will not appear again. 2 = Icon moves with text but appears again when the text starts to scroll again. | 0 |


All keys are optional, so you can send just the properties you want to use.

To update a custom page, simply send a modified JSON object to the same topic. The display will be updated immediately.

You can also send a one-time notification with the same JSON format. Simply send your JSON object to "awtrixlight/notify".

## Example

Here's an example JSON object to display the text "Hello, AWTRIX Light!" with the icon ID "1", in rainbow colors, for 10 seconds:

```json
{
  "text": "Hello, AWTRIX Light!",
  "icon": "1",
  "rainbow": true,
  "duration": 10
}
```

## Delete a custom page
To delelte a custom page simply send a empty payload to the same topic

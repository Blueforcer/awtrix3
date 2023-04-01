# Custom Apps and Notifications

With AWTRIX Light, you can create custom apps or notifications to display your own text and icons.  
  
With MQTT simply send a JSON object to the topic `[PREFIX]/custom/[page]` where [page] is a the name of your page (without spaces).  
With the [HTTP API](api?id=add-custom-app) you have to set the appname in the request header  (`name = Appname`)  

## JSON Properties

The JSON object has the following properties:

| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `pos` | number | defines the position of your custompage in the loop, starting at 0 for the first position. This will only apply with your first push. You cant change the position afterwards with [this function](api?id=addremove-and-rearange-apps) |
| `text` | string | The text to display on the page. | |
| `icon` | string | The icon ID or filename (without extension) to display on the page. | |
| `repeat` | number | Sets how many times the text should be scrolled through the matrix before the display ends. | 1 |
| `rainbow` | boolean | Fades each letter in the text differently through the entire RGB spectrum. | false |
| `duration` | number | Sets how long the notification should be displayed. | 5 |
| `color` | string | A color hex string for the text color, or an array of R,G,B values | "#FFFFFF" or [255,255,0] |
| `hold` | boolean | Set it to true, to hold your notification on top until you press the middle button or dismiss it via HomeAssistant. This key only belongs to notification. | false |
| `sound` | string | The filename of your RTTTL ringtone file (without extension). | |
| `pushIcon` | number | 0 = Icon doesn't move. 1 = Icon moves with text and will not appear again. 2 = Icon moves with text but appears again when the text starts to scroll again. | 0 |
| `bar` | array of integers | draws a bargraph. Without icon maximum 16 values, with icon 11 values |  |


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

## Delete a custom app
To delete a custom app simply send a empty payload/body to the same topic/url.
You can also use [this API](api?id=addremove-and-rearange-apps)
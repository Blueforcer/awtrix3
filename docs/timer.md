# Timer

With AWTRIX Light, you can set a timer using MQTT. Simply send a JSON object to the topic **[PREFIX]/timer** to start a timer. 

When the timer goes off, the display will show a notification, and you can dismiss the timer by pressing the middle button. 

## JSON Properties

The JSON object has the following properties:

| Key | Type | Description |
| --- | ---- | ----------- |
| `hours` | number | The number of hours after midnight when the timer should be triggered. |
| `minutes` | number | The number of minutes after the hour when the timer should be triggered. |
| `seconds` | number | The number of seconds after the minute when the timer should be triggered. |
| `sound` | string | The name of the sound file (without extension) to play when the timer is triggered. |

Each value is optional, so you can set a timer for just minutes, or any combination of hours, minutes, and seconds. If you only want to start a timer in some minutes, just send the minutes.

## Example

Here's an example JSON object to start a timer for 1 hour, 30 minutes, and 10 seconds, with the sound "friends":

```json
{  
  "hours": 1,  
  "minutes": 30,  
  "seconds": 10,  
  "sound": "friends"  
}
```

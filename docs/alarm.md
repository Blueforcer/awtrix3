# Alarm Clock

AWTRIX Light now includes an alarm clock feature that allows you to set alarms using an `alarms.json` file.
The JSON object contains an array of alarms with different properties, which are explained below.   
To create a new alarm, add a new object to the "alarms" array with the desired properties, following the same format as the existing alarms. You do not need to restart AWTRIX in order to update your alarms.  

## JSON Properties

| Key | Type | Description |
| --- | ---- | ----------- |
| `hour` | number | The hour at which the alarm should go on, specified in 24-hour format. |
| `minute` | number | The minute at which the alarm should go on. |
| `days` | string | A string representing the days on which the alarm should go off, where "0" represents Monday, "1" represents Tuesday, and so on up to "6" for Sunday. Multiple days can be specified as a string of digits, for example "0123456" would represent an alarm set for every day of the week. |
| `sound` | string | An optional string representing the name of the soundfile (without extension) to play when the alarm starts. This sound repeats as long the alarm is on. |
| `snooze` | number | An optional integer representing the number of minutes for which the alarm should be snoozed when the snooze button is pressed. |




## Using the Alarm Clock

To set an alarm, create an `alarms.json` with the integrated filebrowser and add one or more alarm objects. When the alarm starts, press the middle button to snooze for the specified amount of time, or hold the middle button to turn off the alarm. 

Example `alarms.json`:

```json
{
  "alarms": [
    {
      "hour": 07,
      "minute": 30,
      "days": "01234",
      "sound": "bond",
      "snooze": 10
    },
    {
      "hour": 10,
      "minute": 0,
      "days": "56"
    }
  ]
}

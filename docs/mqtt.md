# MQTT Commands

## Dismiss Notification  
Dismiss a notification which was set to "hold"=true.

| Topic | Payload |
| --- | --- |
| `[PREFIX]/notify/dismiss` | empty payload |

## Switch Apps
Switch to next or previous app.

| Topic | Payload |
| --- | --- |
| `[PREFIX]/nextapp` | empty payload |
| `[PREFIX]/previousapp` | empty payload |

## Switch to Specific App  
Switch to a specific app by name.

| Topic | Payload |
| --- | --- |
| `[PREFIX]/switch` | `{"name":"time"}` |

Built-in app names are:
- `time`
- `date`
- `temp`
- `hum`
- `bat`

For custom pages, use the name you set in the topic. For example, if `[PREFIX]/custom/test` is your topic, then `test` is the name.

## Change Settings  
Change various settings related to the app display.

| Topic | Payload |
| --- | --- |
| `[PREFIX]/settings` | JSON properties |

Each property is optional; you do not need to send all.

| Key | Type | Description | Value Range | Default |
| --- | --- | --- | --- | --- |
| `apptime` | number | Determines the duration an app is displayed in milliseconds. | Any positive integer value. | 7000 |
| `transition` | number | The time the transition to the next app takes in milliseconds. | Any positive integer value. | 500 |
| `textcolor` | string | A color in hexadecimal format. | Any valid 6-digit hexadecimal color value, e.g. "#FF0000" for red. | N/A |
| `fps` | number | Determines the frame rate at which the matrix is updated. | Any positive integer value. | 23 |
| `brightness` | number | Determines the brightness of the matrix. | An integer between 0 and 255. | N/A |
| `autobrightness` | boolean | Determines if automatic brightness control is active. | `true` or `false`. | N/A |
| `autotransition` | boolean | Determines if automatic switching to the next app is active. | `true` or `false`. | N/A |

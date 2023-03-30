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

For custom apps, use the name you set in the topic. For example, if `[PREFIX]/custom/test` is your topic, then `test` is the name.

## Add/remove and rearange apps 


| Topic | 
| --- |
| `[PREFIX]/apps` |

!> This function provides users with the ability to manage the apps on their device by adding, removing, and rearranging them. However, as it is an experimental feature, caution should be exercised, particularly when attempting to rearrange multiple apps at once, as this can lead to unintended consequences due to the resulting shifts in position of other apps.
  
By using this function, users can add or remove native apps, as well as custom apps, from the device.  
However, it is important to note that custom apps are only temporarily loaded into memory and cannot be added again  using this  function.  
To add a custom app again, you must send it to awtrix via mqtt again.  
  
Additionally, you can rearrange the position of all apps on the device by specifying a new position in the JSON array.   
This provides flexibility in organizing apps according to personal preference.    

The JSON payload is an array of objects, where each object represents an app to be displayed on awtrix. Each app object contains the following fields:

`"name"`: The name of the app ("time", "date", "temp", "hum", "bat") are the native apps.  
For custom apps, use the name you set in the topic. For example, if `[PREFIX]/custom/test` is your topic, then `test` is the name.    
`"show"`: A boolean indicating whether the app should be shown on the screen or not. If not present, the app is considered active by default.  
`"pos"`: An integer indicating the position of the app in the list. If not present, the app will be added to the end of the list.  

> You can also just send the information for one app.

```json
[
   {
      "name":"time",
      "show":true,
      "pos":3
   },
   {
      "name":"date",
      "pos":0
   },
   {
      "name":"temp",
      "pos":2
   },
   {
      "name":"hum",
      "show":true,
      "pos":0
   },
   {
      "name":"bat",
      "show":false
   },
   {
      "name":"github",
      "show":true,
      "pos":4
   }
]
```


  
In this example,
- The "time" app is active and should be displayed in position 3.  
- The "date" app should be displayed in position 0.  
- The "temp" app should be displayed in position 2.  
- The "hum" app should be displayed at first position.  
- The "bat" app is inactive and will be removed,   
- and the "github" app is active and should be displayed in position 4.  




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

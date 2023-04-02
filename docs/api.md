# MQTT / HTTP API


## Status  
In MQTT awtrix send its stats every 10s to `[PREFIX]/stats`  
With HTTP, make GET request to `http://[IP]/api/stats`


## Update  
Awtrix searches for an update every 1 Hour. If a new one is found it will be published to HA and in the stats.  
You can start the update with update button in HA or:   
 
| Topic | URL | Payload/Body | HTTP Header | HTTP method |  
| --- | --- | --- | --- | --- |  
| `[PREFIX]/doupdate` |`http://[IP]/api/doupdate` | JSON | empty payload/body | POST |  
  
## Add custom app  
 create custom apps or notifications to display your own text and icons.  
 Have a look at [this section](custom?id=custom-apps-and-notifications)

| Topic | URL |  Payload/Body | Query parameters | HTTP method |
| --- | --- | --- | --- | --- |
| `[PREFIX]/custom/[appname]` |`http://[IP]/api/custom` | JSON | name = [appname] | POST |

## Dismiss Notification  
Dismiss a notification which was set to "hold"=true.

| Topic | URL | Payload/Body | HTTP method |
| --- | --- | --- | --- |
| `[PREFIX]/custom/[appname]` |`http://[IP]/api/notify/dismiss` | empty payload/body | POST |

## Switch Apps
Switch to next or previous app.

| Topic | URL | Payload/Body | HTTP method |
| --- | --- | --- | --- |
| `[PREFIX]/nextapp` | `http://[IP]/api/nextapp` | empty payload/body | POST |
| `[PREFIX]/previousapp` | `http://[IP]/api/previousapp` | empty payload/body  | POST |

## Switch to Specific App  
Switch to a specific app by name.

| Topic | URL | Payload/Body | HTTP method |
| --- | --- | --- | --- |
| `[PREFIX]/switch` | `http://[IP]/api/switch` | `{"name":"time"}` | POST |

Built-in app names are:
- `time`
- `date`
- `temp`
- `hum`
- `bat`

For custom apps, use the name you set in the topic or http request header.  
In MQTT for example, if `[PREFIX]/custom/test` is your topic, then `test` is the name.

## Add/remove and rearange apps 


| Topic |  URL | Payload/Body | HTTP method |
| --- | --- | --- | --- |
| `[PREFIX]/apps`|`http://[IP]/api/apps`| json | POST |

!> This function provides users with the ability to manage the apps on their device by adding, removing, and rearranging them. However, as it is an experimental feature, caution should be exercised, particularly when attempting to rearrange multiple apps at once, as this can lead to unintended consequences due to the resulting shifts in position of other apps.
  
By using this function, users can add or remove native apps, as well as custom apps, from the device.  
However, it is important to note that custom apps are only temporarily loaded into memory and cannot be added again  using this  function.  
To add a custom app again, you must send it to awtrix via mqtt again.  
  
Additionally, you can rearrange the position of all apps on the device by specifying a new position in the JSON array.   
This provides flexibility in organizing apps according to personal preference.    

The JSON payload is an array of objects, where each object represents an app to be displayed on awtrix. Each app object contains the following fields:

### JSON Properties

| Property | Description |Default |
|----------|-------------|-------------|
| name     | The name of the app. If it's a native app, it can be one of "time", "date", "temp", "hum", or "bat". For custom apps, use the name you set in the topic. For example, if `[PREFIX]/custom/test` is your topic, then `test` is the name. | |
| show     | A boolean indicating whether the app should be shown on the screen or not. If not present, the app is considered active by default. | true |
| pos      | An integer indicating the position of the app in the list. If not present, the app will be added to the end of the list. | Last Item |


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

## Add timer  
 create custom apps or notifications to display your own text and icons.
 have a look at [this section](custom)

| Topic | URL | Payload/Body |HTTP method |
| --- | --- | --- |--- |
| `[PREFIX]/custom/[appname]` |`http://[IP]/api/notify/dismiss` | empty payload/body | POST |


## Change Settings  
Change various settings related to the app display.

| Topic |  URL | Payload/Body |HTTP method |
| --- | --- | --- |--- |
| `[PREFIX]/settings` |`http://[IP]/api/settings`| JSON | POST |


#### JSON Properties
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


## Timer

With AWTRIX Light, you can set a timer using MQTT. Simply send a JSON object to the topic **[PREFIX]/timer** to start a timer. 

When the timer goes off, the display will show a notification, and you can dismiss the timer by pressing the middle button. 

#### JSON Properties

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


# MQTT / HTTP API
  
## Status  
In MQTT awtrix send its stats every 10s to `[PREFIX]/stats`  
With HTTP, make GET request to `http://[IP]/api/stats`
  
  
## Turn display on or off    
  
| Topic | URL | Payload/Body | HTTP method |  
| --- | --- | --- | --- |  
| `[PREFIX]/power` | `http://[IP]/api/power` | true/false or 1/0 | POST |  


## Colored indicators     

A colored indicator is like a small notification sign wich will be shown on the upper right or lower right corner.  

| Topic | URL | Payload/Body | HTTP method |  
| --- | --- | --- | --- |  
| `[PREFIX]/indicator1` | `http://[IP]/api/indicator1` | `{"color":[255,0,0]}` | POST |  
| `[PREFIX]/indicator2` | `http://[IP]/api/indicator2` | `{"color":[0,255,0]}` | POST |  

Instead of a RGB array you can also sent HEX color strings like `{"color":"#32a852"}`  
Send the color black  `{"color":[0,0,0]}` or `{"color":"0"}` to hide the indicators.    
Optionally you can make the indicator blinking by adding the key `"blink":true/false`.   
  
## Custom Apps and Notifications
With AWTRIX Light, you can create custom apps or notifications to display your own text and icons.  
  
With MQTT simply send a JSON object to the topic `[PREFIX]/custom/[app]` where [app] is a the name of your app (without spaces).  
With the HTTP API you have to set the appname in the request header  (`name = [appname]`)  
To update a custom page, simply send a modified JSON object to the same endpoint. The display will be updated immediately.  

You can also send a one-time notification with the same JSON format. Simply send your JSON object to `[PREFIX]/notify` or `http://[IP]/api/notify`.  
  
  
| Topic | URL |  Payload/Body | Query parameters | HTTP method |
| --- | --- | --- | --- | --- |
| `[PREFIX]/custom/[appname]` |`http://[IP]/api/custom` | JSON | name = [appname] | POST |
| `[PREFIX]/notify` |`http://[IP]/notify` | JSON | - | POST |


### JSON Properties

The JSON object has the following properties,  
All keys are optional, so you can send just the properties you want to use.

| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `text` | string | The text to display on the app. | N/A |
| `icon` | string | The icon ID or filename (without extension) to display on the app. | N/A |
| `repeat` | number | Sets how many times the text should be scrolled through the matrix before the app ends. | 1 |
| `rainbow` | boolean | Fades each letter in the text differently through the entire RGB spectrum. | false |
| `duration` | number | Sets how long the app or notification should be displayed. | 5 |
| `color` | string | A color hex string for the text color, or an array of R,G,B values | "#FFFFFF" or [255,255,0] |
| `hold` | boolean | Set it to true, to hold your **notification** on top until you press the middle button or dismiss it via HomeAssistant. This key only belongs to notification. | false |
| `sound` | string | The filename of your RTTTL ringtone file (without extension). | N/A |
| `pushIcon` | number | 0 = Icon doesn't move. 1 = Icon moves with text and will not appear again. 2 = Icon moves with text but appears again when the text starts to scroll again. | 0 |
| `bar` | array of integers | draws a bargraph. Without icon maximum 16 values, with icon 11 values | N/A |
| `line` | array of integers | draws a linechart. Without icon maximum 16 values, with icon 11 values | N/A |
| `autoscale` | boolean | enables or disables autoscaling for bar and linechart | true |
| `lifetime` | integer | Removes the custom app when there is no update after the given time in seconds | 0 |
| `textCase` | integer | Changes the Uppercase setting. 0=global setting, 1=forces uppercase; 2=shows as it sent. | 0 |
| `textOffset` | integer | Sets an offset for the x position of a starting text. | 0 |
| `pos` | number | defines the position of your custompage in the loop, starting at 0 for the first position. This will only apply with your first push. You cant change the position afterwards with [this function](api?id=addremove-and-rearange-apps) | N/A |

### Example

Here's an example JSON object to display the text "Hello, AWTRIX Light!" with the icon name "1", in rainbow colors, for 10 seconds:

```json
{
  "text": "Hello!",
  "rainbow": true,
  "duration": 10
}
```

### Display a text in colored fragments
You can display a text where you allowed to colorize fragments of the text.  
Simply send an array of your fragments, containing `"t"` as your textfragment and `"c"` for the color hex value`.  

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
  
## Delete a custom app
To delete a custom app simply send a empty payload/body to the same topic/url.
You can also use [this API](api?id=addremove-and-rearange-apps)

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



## Change Settings  
Change various settings related to the app display.

| Topic |  URL | Payload/Body |HTTP method |
| --- | --- | --- |--- |
| `[PREFIX]/settings` |`http://[IP]/api/settings`| JSON | POST |


#### JSON Properties
Each property is optional; you do not need to send all.

| Key | Type | Description | Value Range | Default |
| --- | --- | --- | --- | --- |
| `ATIME` | number | Determines the duration an app is displayed in milliseconds. | Any positive integer value. | 7000 |
| `TSPEED` | number | The time the transition to the next app takes in milliseconds. | Any positive integer value. | 500 |
| `TCOL` | string / array of ints| Sets the textcolor | an array of RGB values `[255,0,0]` or any valid 6-digit hexadecimal color value, e.g. "#FF0000" for red. | N/A |
| `WD` | bool | Enable or disable the weekday display | true/false | true |
| `WDCA` | string / array of ints| Sets the active weekday color | an array of RGB values `[255,0,0]` or any valid 6-digit hexadecimal color value, e.g. "#FF0000" for red. | N/A |
| `WDCI` | string / array of ints| Sets the inactive weekday color | an array of RGB values `[255,0,0]` or any valid 6-digit hexadecimal color value, e.g. "#FFFF" for red. | N/A |
| `FPS` | number | Determines the frame rate at which the matrix is updated. | Any positive integer value. | 23 |
| `BRI` | number | Determines the brightness of the matrix. | An integer between 0 and 255. | N/A |
| `ABRI` | boolean | Determines if automatic brightness control is active. | `true` or `false`. | N/A |
| `ATRANS` | boolean | Determines if automatic switching to the next app is active. | `true` or `false`. | N/A |
| `CCORRECTION` | array of ints | Sets the color correction for the matrix | an array of RGB values | N/A |
| `CTEMP` | array of ints | Sets the color temperature for the matrix | an array of RGB values | N/A |
| `GAMMA` | float | Sets the gamma for the matrix | 2.5 | N/A |
| `TFORMAT` | string | Sets the timeformat for the TimeApp | see below | N/A |
| `DFORMAT` | string | Sets the dateformat for the DateApp | see below | N/A |
| `SOM` | bool | Sets the start of the week to monday | true/false | true |
| `BLOCKN` | bool | Blocks temporarily the physical navigation keys, but still sends the input to MQTT | true/false | false |
| `UPPERCASE` | bool | Shows text in uppercase | true/false | true |


**Timeformats:**  
```bash  
%H:%M:%S     13:30:45  
%l:%M:%S     1:30:45  
%H:%M        13:30  
%H %M        13.30 with blinking colon  
%l:%M        1:30  
%l %M        1:30 with blinking colon  
%l:%M %p     1:30 PM  
%l %M %p     1:30 PM with blinking colon  
```
  
**Dateformats:**  
```bash  
%d.%m.%y     01.04.22  
%d.%m        01.04  
%y-%m-%d     22-04-01  
%m-%d        04-01  
%m/%d/%y     04/01/22  
%m/%d        04/01  
%d/%m/%y     01/04/22  
%d/%m        01/04  
%m-%d-%y     04-01-22  
```  
  
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

#### Example

Here's an example JSON object to start a timer for 1 hour, 30 minutes, and 10 seconds, with the sound "friends":

```json
{  
  "hours": 1,  
  "minutes": 30,  
  "seconds": 10,  
  "sound": "friends"  
} 
```
  
## Update  
Awtrix searches for an update every 1 Hour. If a new one is found it will be published to HA and in the stats.  
You can start the update with update button in HA or:   
   
| Topic | URL | Payload/Body | HTTP Header | HTTP method |  
| --- | --- | --- | --- | --- |  
| `[PREFIX]/doupdate` |`http://[IP]/api/doupdate` | JSON | empty payload/body | POST |  
  
  
## Reboot Awtrix    
  
| Topic | URL | Payload/Body | HTTP method |  
| --- | --- | --- | --- |  
| `[PREFIX]/reboot` | `http://[IP]/api/reboot` | - | POST |  
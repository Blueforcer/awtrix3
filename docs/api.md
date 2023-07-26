# MQTT / HTTP API
  
## Status  
In MQTT awtrix send its stats every 10s to `[PREFIX]/stats`  
With HTTP, make GET request to `http://[IP]/api/stats`
  
  
## Turn display on or off    
  
| Topic | URL | Payload/Body | HTTP method |  
| --- | --- | --- | --- |  
| `[PREFIX]/power` | `http://[IP]/api/power` | {"power":true} or {"power":false}  | POST |  


## Play a sound

Plays a RTTTL sound from the MELODIES folder.  
    
| Topic | URL | Payload/Body | HTTP method |  
| --- | --- | --- | --- |  
| `[PREFIX]/sound` | `http://[IP]/api/sound` | {"sound":"alarm"} | POST |  
  

## Moodlight  
Allows to set the whole matrix to a custom color or temperature.  
  
!> This function causes much higher current draw and heat, because every pixel is lit. Keep calm with the brightness value!  
  
Send empty payload to disable moodlight.  
  
| Topic | URL | Payload/Body | HTTP method |  
| --- | --- | --- | --- |  
| `[PREFIX]/moodlight` | `http://[IP]/api/moodlight` | see below | POST |  
  

Possible moodlight options:  
```json
{"brightness":170,"kelvin":2300}  
or
{"brightness":170,"color":[155,38,182]}  
or
{"brightness":170,"color":"#FF00FF"}  
```
  

## Colored indicators     

A colored indicator is like a small notification sign wich will be shown on the upper right (1), right side (2) and lower right corner (3).  

| Topic | URL | Payload/Body | HTTP method |  
| --- | --- | --- | --- |  
| `[PREFIX]/indicator1` | `http://[IP]/api/indicator1` | `{"color":[255,0,0]}` | POST |  
| `[PREFIX]/indicator2` | `http://[IP]/api/indicator2` | `{"color":[0,255,0]}` | POST |  
| `[PREFIX]/indicator3` | `http://[IP]/api/indicator3` | `{"color":[0,255,0]}` | POST |  

Instead of a RGB array you can also sent HEX color strings like `{"color":"#32a852"}`  
Send the color black  `{"color":[0,0,0]}` or `{"color":"0"}` or a empty payload/body to hide the indicators.    
Optionally you can make the indicator blinking by adding the key `"blink"` with a value of the blinking interval in milliseconds.   
  
## Custom Apps and Notifications
With AWTRIX Light, you can create custom apps or notifications to display your own text and icons.  
  
With MQTT simply send a JSON object to the topic `[PREFIX]/custom/[app]` where [app] is a the name of your app (without spaces).  
With the HTTP API you have to set the appname in the query parameter  (`name = [appname]`)  
To update a custom page, simply send a modified JSON object to the same endpoint. The display will be updated immediately.  

You can also send a one-time notification with the same JSON format. Simply send your JSON object to `[PREFIX]/notify` or `http://[IP]/api/notify`.  
  
  
| Topic | URL |  Payload/Body | Query parameters | HTTP method |
| --- | --- | --- | --- | --- |
| `[PREFIX]/custom/[appname]` |`http://[IP]/api/custom` | JSON | name = [appname] | POST |
| `[PREFIX]/notify` |`http://[IP]/api/notify` | JSON | - | POST |


### JSON Properties

The JSON object has the following properties,  
**All keys are optional**, so you can send just the properties you want to use.

| Key | Type | Description | Default | Custom App | Notification |
| --- | ---- | ----------- | ------- | ------- |------- |
| `text` | string | The text to display. | N/A | X | X |
| `textCase` | integer | Changes the Uppercase setting. 0=global setting, 1=forces uppercase; 2=shows as it sent. | 0 | X | X |
| `topText` | boolean | Draw the text on top | false | X | X |
| `textOffset` | integer | Sets an offset for the x position of a starting text. | 0 | X | X |
| `color` | string or array of integers | The text, bar or line color | | X | X |
| `background` | string or array of integers | Sets a background color | | X | X |
| `rainbow` | boolean | Fades each letter in the text differently through the entire RGB spectrum. | false | X | X |
| `icon` | string | The icon ID or filename (without extension) to display on the app. | N/A | X | X |
| `pushIcon` | integer | 0 = Icon doesn't move. 1 = Icon moves with text and will not appear again. 2 = Icon moves with text but appears again when the text starts to scroll again. | 0 | X | X |
| `repeat` | integer | Sets how many times the text should be scrolled through the matrix before the app ends. | 1 | X | X |
| `duration` | integer | Sets how long the app or notification should be displayed. | 5 | X | X |
| `hold` | boolean | Set it to true, to hold your **notification** on top until you press the middle button or dismiss it via HomeAssistant. This key only belongs to notification. | false |   | X |
| `sound` | string | The filename of your RTTTL ringtone file placed in the MELODIES folder (without extension). | N/A |   | X |
| `rtttl` | string | Allows to send the RTTTL sound string with the json |  |   | X |
| `loopSound` | boolean | Loops the sound or rtttl as long as the notification is running | false |   | X |
| `bar` | array of integers | draws a bargraph. Without icon maximum 16 values, with icon 11 values | N/A | X | X |
| `line` | array of integers | draws a linechart. Without icon maximum 16 values, with icon 11 values | N/A | X | X |
| `autoscale` | boolean | Enables or disables autoscaling for bar and linechart | true | X | X |
| `progress` | integer | Shows a progressbar. Value can be 0-100 | -1 | X | X |
| `progressC` | string or array of integers  | The color of the progressbar | -1 | X | X |
| `progressBC` | string or array of integers  | The color of the progressbar background | -1 | X | X |
| `pos` | integer | Defines the position of your custompage in the loop, starting at 0 for the first position. This will only apply with your first push. This function is experimental | N/A |  X |   | 
| `draw` | array of objects | Array of drawing instructions. Each object represents a drawing command. | See the drawing instructions below | X | X |
| `lifetime` | integer | Removes the custom app when there is no update after the given time in seconds | 0 | X |   |
| `stack` | boolean | Defines if the **notification** will be stacked. false will immediately replace the current notification | true |   | X |
| `wakeup` | boolean | If the Matrix is off, the notification will wake it up for the time of the notification. | false |   | X |
| `noScroll` | boolean | Disables the textscrolling | false | X | X |
| `clients` | array of strings | Allows to forward a notification to other awtrix. Use the MQTT prefix for MQTT and IP adresses for HTTP |  |   | X |
| `scrollSpeed` | integer | Modifies the scrollspeed. You need to enter a percentage value | 100 | X | X |
| `effect` | string | Shows an [effect](https://blueforcer.github.io/awtrix-light/#/effects) as background |  | X | X |  
| `save` | boolean | Saves your customapp into flash and reload it after boot. You should avoid that with customapps wich has high update frequency because ESPs flashmemory has limited writecycles  |  | X |  |  
  

Color values can have a hex string or an array of R,G,B values:  
`"#FFFFFF" or [255,255,0]`  
  
#### Example

Here's an example JSON object to display the text "Hello, AWTRIX Light!" with the icon name "1", in rainbow colors, for 10 seconds:

```json
{
  "text": "Hello!",
  "rainbow": true,
  "duration": 10
}
```

  
### Drawing Instructions
!> Please note: Depending on the number of objects, the RAM usage can be very high. This could cause freezes or reboots.
It's important to be mindful of the number of objects and the complexity of the drawing instructions to avoid performance issues.  
  
Each drawing instruction is an object with a required command key and an array of values depending on the command:  
  
| Command | Array Values         | Description |
| ------- | -------------------- | ----------- |
| `dp`    | `[x, y, cl]`         | Draw a pixel at position (`x`, `y`) with color `cl` |
| `dl`    | `[x0, y0, x1, y1, cl]` | Draw a line from (`x0`, `y0`) to (`x1`, `y1`) with color `cl` |
| `dr`    | `[x, y, w, h, cl]`   | Draw a rectangle with top-left corner at (`x`, `y`), width `w`, height `h`, and color `cl` |
| `df`    | `[x, y, w, h, cl]`   | Draw a filled rectangle with top-left corner at (`x`, `y`), width `w`, height `h`, and color `cl` |
| `dc`    | `[x, y, r, cl]`      | Draw a circle with center at (`x`, `y`), radius `r`, and color `cl` |
| `dfc`   | `[x, y, r, cl]`      | Draw a filled circle with center at (`x`, `y`), radius `r`, and color `cl` |
| `dt`    | `[x, y, t, cl]`      | Draw text `t` with top-left corner at (`x`, `y`) and color `cl` |
| `db`    | `[x, y, w, h, [bmp]]`    | Draws a RGB565 bitmap array `[bmp]` with top-left corner at (`x`, `y`) and size of (`w`, `h`) |
  
### Example    
  
Here's an example JSON object to draw a red circle, a blue rectangle, and the text "Hello" in green:  
  
```json   
{"draw":[  
 {"dc": [28, 4, 3, "#FF0000"]},  
 {"dr": [20, 4, 4, 4, "#0000FF"]},  
 {"dt": [0, 0, "Hello", "#00FF00"]}  
]}  
```  

### Send multiple custompage at once  
This allows you to send multiple custompage at once. Instead of a single custompage object, you can send an array of objects. like

**Topic:**   
/custom/test

```json 
`[{"text":"1"},{"text":"2"}]`  
```  
Internally the appname gets a suffix like test0, test1 and so on.  
You can update each app like before, or you can update all apps at once.  
While removing apps, awtrix doesnt search for the exact name, but uses the app that starts with the given name.  
So if you want to delete all apps just send empty payload to /custom/test. This will remove test0, test1 and so on.  
you can also remove one app by removing /custom/test1.  
Please keep in mind that if you delete only one app, you cant get the correct Order again because all apps in the loop move, since there can be no placeholder.  
  
  
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

## Dismiss Notification  
Dismiss a notification which was set to "hold"=true.

| Topic | URL | Payload/Body | HTTP method |
| --- | --- | --- | --- |
| `[PREFIX]/notify/dismiss` |`http://[IP]/api/notify/dismiss` | empty payload/body | POST |

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


## Change Settings  
Change various settings related to the app display.

| Topic |  URL | Payload/Body |HTTP method |
| --- | --- | --- |--- |
| `[PREFIX]/settings` |`http://[IP]/api/settings`| JSON | POST |


#### JSON Properties
Each property is optional; you do not need to send all.

| Key | Type | Description | Value Range | Default |
| --- | --- | --- | --- | --- |
| `ATIME` | number | Determines the duration an app is displayed in seconds. | Any positive integer value. | 7 |
| `TSPEED` | number | The time the transition to the next app takes in milliseconds. | Any positive integer value. | 500 |
| `TCOL` | string / array of ints| Sets the textcolor | an array of RGB values `[255,0,0]` or any valid 6-digit hexadecimal color value, e.g. "#FF0000" for red. | N/A |
| `WD` | bool | Enable or disable the weekday display | true/false | true |
| `WDCA` | string / array of ints| Sets the active weekday color | an array of RGB values `[255,0,0]` or any valid 6-digit hexadecimal color value, e.g. "#FF0000" for red. | N/A |
| `WDCI` | string / array of ints| Sets the inactive weekday color | an array of RGB values `[255,0,0]` or any valid 6-digit hexadecimal color value, e.g. "#FFFF" for white. | N/A |
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
| `TIME_COL` | string / array of ints| Sets the textcolor of the time app. Set 0 for global textcolor | an array of RGB values hexadecimal color value | N/A |
| `DATE_COL` | string / array of ints| Sets the textcolor of the date app . Set 0 for global textcolor | an array of RGB values hexadecimal color value | N/A |
| `TEMP_COL` | string / array of ints| Sets the textcolor of the temp app. Set 0 for global textcolor  | an array of RGB values hexadecimal color value | N/A |
| `HUM_COL` | string / array of ints| Sets the textcolor of the humidity app. Set 0 for global textcolor  | an array of RGB values hexadecimal color value | N/A |
| `BAT_COL` | string / array of ints| Sets the textcolor of the battery app. Set 0 for global textcolor  | an array of RGB values hexadecimal color value | N/A |
| `SSPEED` | integer | Modifies the scrollspeed | percentage value of the original scrollspeed | 100 |

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
%d.%m.%y     16.04.22  
%d.%m        16.04  
%y-%m-%d     22-04-16  
%m-%d        04-16  
%m/%d/%y     04/16/22  
%m/%d        04/16  
%d/%m/%y     16/04/22  
%d/%m        16/04  
%m-%d-%y     04-16-22  
```  
    
## Update  
You can start the firmware update with update button in HA or:   
   
| Topic | URL | Payload/Body | HTTP Header | HTTP method |  
| --- | --- | --- | --- | --- |  
| `[PREFIX]/doupdate` |`http://[IP]/api/doupdate` | JSON | empty payload/body | POST |  
  
## Reboot Awtrix    
  
| Topic | URL | Payload/Body | HTTP method |  
| --- | --- | --- | --- |  
| `[PREFIX]/reboot` | `http://[IP]/api/reboot` | - | POST |  

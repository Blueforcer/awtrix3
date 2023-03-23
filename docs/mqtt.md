# MQTT Commands

### Switch apps
##### Topic 
`[PREFIX]/nextapp`   
`[PREFIX]/previousapp` 

### Switch to specific app  
##### Topic 
`[PREFIX]/switch`  
##### Payload 
`{"name":"time"}`

Build-in app names are
- `time`
- `date`
- `temp`
- `hum`
- `bat`

For custompages you need to call the name you set in the topic:  
If `[PREFIX]/custom/test` is your topic, 
then `test` is the name.

## Change Settings  
##### Topic  
`[PREFIX]/settings`   
  
Each property is optional, you dont need to send all!  
      
##### JSON Properties
| Key         | Type    | Description                                                                 | Value Range                                |
| ----------- | ------- | --------------------------------------------------------------------------- | ------------------------------------------ |
| `apptime`   | number  | Determines the duration an app is displayed in milliseconds.               | Any positive integer value. Default 7000                 |
| `transition`| number  | The time the transition to the next app takes in milliseconds.             | Any positive integer value. Default 500                 |
| `textcolor` | string  | A color in hexadecimal format.                                             | Any valid 6-digit hexadecimal color value, e.g. "#FF0000" for red |
| `fps`       | number  | Determines the frame rate at which the matrix is updated.                 | Any positive integer value. Default 23                  |
| `brightness`| number  | Determines the brightness of the matrix.                                   | An integer between 0 and 255                |
| `autobrightness`| boolean | Determines if automatic brightness control is active.                    | `true` or `false`                          |
| `autotransition`| boolean | Determines if automatic switching to the next app is active.               | `true` or `false`                          |

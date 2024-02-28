# DIY Hardware

if you want to build your own AWTRIX Light, here are the pinout of the Ulanzi clock  
  

|ESP32 PIN | GPIO  | Usage or part                          |
|---|-------|-----------------------------------------------|
|6| ADC6 / GPIO34    | Battery sensor                      |
|7 | ADC7 / GPIO35   | LDR (light) sensor (GL5516)          |
|8| GPIO32    | Matrix                                      |
|11| GPIO26    | Left button                                |
|12| GPIO27    | Middle button                              |
|13| GPIO14    | Right button                               |
|23| GPIO15    | Buzzer                                     |
|33/36| 21/22 | Temperature and Humidity Sensors (SHT3x)    |




If the matrix displays meaningless characters, the matrix type must be changed.

Create a dev.json in your filemanager with the following content:

```json
{
  "matrix": 2
}
```

Change the matrix layout to 0,1 or 2

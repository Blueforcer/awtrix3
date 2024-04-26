# DIY Hardware

if you want to build your own AWTRIX 3, here are the pinout of the Ulanzi clock  
  

|ESP32 PIN | GPIO  | Usage or part                          |
|---|-------|-----------------------------------------------|
|6  | ADC6 / GPIO34    | Battery sensor (optional)                     |
|7  | ADC7 / GPIO35   | LDR (light) sensor (GL5516) (optional)          |
|8  | GPIO32    | Matrix                                      |
|11 | GPIO26    | Left button (optional)                               |
|12 | GPIO27    | Middle button (optional)                             |
|13 | GPIO14    | Right button  (optional)                              |
|23 | GPIO15    | Buzzer  (optional)                                    |
|33/36| 21 (SDA) /22(SCL)  | Temperature and Humidity Sensors (optional)     |
|37/30| 23(RX)/18(TX) | DFplayer (optional)     |

Supported Sensors:
BME280
BMP280
HTU21df
SHT31


If the matrix displays meaningless characters, the matrix type must be changed.

Create a dev.json in your filemanager with the following content:

```json
{
  "matrix": 2
}
```

Change the matrix layout to 0,1 or 2

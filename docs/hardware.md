# DIY Hardware

If you want to build your own AWTRIX 3, here are the pinouts:

## Ulanzi TC001

|ESP32 PIN | GPIO  | Usage or part                          |
|---|-------|-----------------------------------------------|
|6  | ADC6 / GPIO34    | Battery sensor (optional)                     |
|7  | ADC7 / GPIO35   | LDR (light) sensor (GL5516) (optional)          |
|8  | GPIO32    | Matrix                                      |
|11 | GPIO26    | Left button (optional)                               |
|12 | GPIO27    | Middle button (optional) NC/Inverted                  |
|13 | GPIO14    | Right button  (optional)                              |
|23 | GPIO15    | Buzzer  (optional)                                    |
|33/36| 21 (SDA) /22(SCL)  | Temperature and Humidity Sensors (optional)     |
|37/30| 23(RX)/18(TX) | DFplayer (optional)     |

## awtrix2_upgrade (WEMOS_D1_MINI32-Environment)

|GPIO  | Usage or part                          |
|-------|-----------------------------------------------|
| -- | Battery sensor (disabled)                        |
| A0 (SVP) / GPIO36   | LDR (light) sensor (GL5516) (optional)          |
| D2 / GPIO21    | Matrix                                               |
| D0 / GPIO26    | Left button (optional)                               |
| D4 / GPIO16    | Middle button (optional) NC/Inverted                 |
| D8 / GPIO15    | Right button  (optional)                             |
| --   | Buzzer  (disabled)                                             |
| D3 21 (SDA) / D1 22(SCL)  | Temperature and Humidity Sensors (optional)     |
| 23(RX)/18(TX) | DFplayer (optional)     |

## Supported Sensors:

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


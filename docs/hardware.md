# DIY Hardware

if you want to build your own AWTRIX 3, here are the pinout of the Ulanzi clock  
  

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

Building with an ESP8266 (Wemos D1 mini or NodeMCU) instead? It uses a different pinout, see [ESP8266 build](esp8266.md#pinout).

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

## 16x16 panel

A 16x16 WS2812B panel has the same 256 LEDs as the 32x8 matrix, so it works on any build (ESP32 or ESP8266) with matrix layout `3`. The display is split in two: the left half of the normal 32x8 screen shows on the top 8 rows and the right half on the bottom 8 rows. Everything works, but text reads like one line wrapped onto two.

16x16 panels differ in how the LEDs are chained, so tell AWTRIX where the data input is and which way the strip runs. Add these keys to `dev.json` (the values below are the defaults):

```json
{
  "matrix": 3,
  "matrix_start": "top-left",
  "matrix_wiring": "rows",
  "matrix_zigzag": true,
  "matrix_test": true
}
```

- `matrix_start`: the corner where the data input is (`top-left`, `top-right`, `bottom-left` or `bottom-right`), with the panel held the way you'll mount it.
- `matrix_wiring`: `rows` if the strip runs left to right along a row, `columns` if it runs up and down a column.
- `matrix_zigzag`: `true` for serpentine panels, where every other row or column runs back the other way (most panels). `false` if every row or column starts on the same side.
- `matrix_test`: shows a test pattern at boot in two steps of 8 seconds each:
  1. The first LED of the strip lights red and the next 15 green. The red LED shows the corner for `matrix_start`, and the green line shows whether the strip runs along a row or a column, for `matrix_wiring`.
  2. Four corners light up through your settings: red top left, green top right, blue bottom left and white bottom right. If any corner is wrong or garbled, fix the settings and restart.

Remove `matrix_test` (or set it to `false`) once the corners are right.

The live view in the web interface still shows the normal 32x8 screen.


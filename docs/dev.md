# Hidden Features

Ok, now they are no longer hidden :).  
This section contains small setting options that the majority of users do not change or change very rarely and therefore saved the effort of creating an elaborate settings interface.

All features are only applied at boot. So you have to restart awtrix after modifying.   

Create a `dev.json` in your filemanager.

## JSON Properties

The JSON object has the following properties:

| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `bootsound` | string | Uses a custom melodie while booting |  |
| `uppercase` | boolean | Print every character in uppercase | `true` |
| `matrix` | int | Changes the matrix layout (0,1 or 2) | `0` |
| `temp_dec_places` | int | Number of decimal places for temperature measurements | `0` |
| `color_correction` | array of int | Sets the colorcorrection of the matrix | `[255,255,255]` |
| `color_temperature` | array of int | Sets the colortemperature of the matrix | `[255,255,255]` |
| `gamma` |float | Sets the gamma of the matrix | `2.5` |
| `update_check` | boolean | Enables searchf or new version every 1 hour. This could cause in loop stack overflow! | `false` |
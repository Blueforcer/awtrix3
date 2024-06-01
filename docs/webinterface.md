# Webinterface

Once AWTRIX is connected to your Wi-Fi network, you can access the web interface via port 80.  
 To do this, simply enter the AWTRIX IP address in your web browser. This IP address is displayed on the matrix at each boot.  
The web interface provides essential settings primarily needed for the initial setup:

## WiFi Setup
In this section, you can configure your Wi-Fi settings and change your access point if necessary.

# Network
This menu allows you to assign a static IP address to AWTRIX.

# MQTT 
Here, you can set up an MQTT broker and enable Home Assistant discovery if you use this smart home system.  

# TIme 
In this section, you can configure your time server and time zone to ensure AWTRIX displays the correct time and date. It also supports automatic daylight saving time adjustments.  

# Icons 
AWTRIX allows you to use icons from LaMetric. To simplify the process, AWTRIX includes an internal downloader.  
 Simply enter the appropriate icon ID, and AWTRIX will download the icon for immediate use.

# Auth
You can secure the web interface and the API with basic authentication. 
Every page, API call, and the AWTRIX app will require these login credentials. 
Do not lose them; otherwise, you will need to reset your AWTRIX completely.

# Files
Access the integrated file manager here. It allows you to upload and download data, such as icons or RTTTL melodies.

# Update
In the "Firmware" section, you can manually upload an update. The required .bin file can be found in the [Release sektion im AWTRIX 3 repo](https://github.com/Blueforcer/awtrix3/releases)

# LiveView
This section allows you to view your AWTRIX matrix live, take screenshots, and create animated GIFs.

# Backup
The backup system compiles the entire flash memory into a zip file and offers it for download. You can restore this backup on another AWTRIX device.

# Docs
A link to the official AWTRIX 3 documentation.

# Flows
A link to the AWTRIX Flows website.

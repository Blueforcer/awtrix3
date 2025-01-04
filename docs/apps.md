# Apps

The AWTRIX 3 system comes equipped with several built-in applications, including Time, Date, Temperature, Humidity, and Battery status.  
As it is designed to integrate seamlessly with your smart home ecosystem, additional applications can be created using MQTT or HTTP requests.

!> In AWTRIX, the term 'Apps' does not refer to traditional smartphone apps that you download and install. Instead, in AWTRIX, CustomApps function more like dynamic pages that rotate within the Apploop rotation of the display. These pages do not store or execute their own logic; instead, they display content that is sent from an external system, such as a smarthome. This content must be transmitted using MQTT or HTTP protocols via the [CustomApp API](https://blueforcer.github.io/awtrix3/#/api?id=custom-apps-and-notifications).  
It is important to note that all the logic for managing the content displayed in these CustomApps needs to be handled by your external system. AWTRIX only provides the platform for displaying the information. You have the flexibility to update the content shown on your CustomApps in real-time at any moment, making it a versatile tool for displaying personalized information in your smarthome setup.

There are numerous benefits to this approach:

- **Personalization:** Customize each application to suit your preferences and needs.
- **Flexibility:** Develop your own applications without the need to modify the firmware.
- **Efficient resource management:** Save valuable flash memory space on the ESP module.
- **Adaptability:** No need to rewrite the firmware if an API undergoes changes.

You can use any system you like which is able to build json strings and send them to a mqtt topic.

## AWTRIX FLOWS
This is your go-to hub for sharing and discovering AWTRIX 3 automations, also known as custom Apps for several services.  
Enhance your AWTRIX 3 experience, exchange ideas, and get inspired. Lets bring our creative automations to life together!  
No login is necessary, neither for creating new flows. As a creator you will get a link with which you can always edit your flow. Keep it save!  
You can upload your icons to your flow, and user can copy them directly to their AWTRIX 3 with one click!  
New flows are regularly moderated.  
https://flows.blueforcer.de/   
    

## Flow example with Node-RED
[Node-RED](https://nodered.org/) serves as an ideal software solution for creating these applications.  
It is available as a standalone program or as a plugin for Home Assistant and ioBroker, allowing you to further enhance the capabilities of your AWTRIX 3 system.
    
Here is a demo, please press the triangle to unfold.

<details>
  <summary>Example for adding a Youtube App as NodeRED flow</summary>    
  <pre><code class="language-json">
[
  {
    "id": "2a59d30d07abe14f",
    "type": "group",
    "z": "54b42d8d.cda474",
    "style": {
      "stroke": "#999999",
      "stroke-opacity": "1",
      "fill": "none",
      "fill-opacity": "1",
      "label": true,
      "label-position": "nw",
      "color": "#a4a4a4"
    },
    "nodes": [
      "f0f17299.3736c",
      "dc7878f9.4756c8",
      "f234aae371d72680",
      "555bb8624b88c9c3",
      "69c388146e28049d",
      "a349ade5a57f7537"
    ],
    "x": 34,
    "y": 39,
    "w": 892,
    "h": 122
  },
  {
    "id": "f0f17299.3736c",
    "type": "inject",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "",
    "props": [],
    "repeat": "3600",
    "crontab": "",
    "once": true,
    "onceDelay": 0.1,
    "topic": "",
    "x": 130,
    "y": 120,
    "wires": [
      [
        "a349ade5a57f7537"
      ]
    ]
  },
  {
    "id": "dc7878f9.4756c8",
    "type": "http request",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "",
    "method": "GET",
    "ret": "obj",
    "paytoqs": "query",
    "url": "https://youtube.googleapis.com/youtube/v3/channels",
    "tls": "",
    "persist": false,
    "proxy": "",
    "insecureHTTPParser": false,
    "authType": "",
    "senderr": false,
    "headers": [],
    "x": 430,
    "y": 120,
    "wires": [
      [
        "f234aae371d72680"
      ]
    ]
  },
  {
    "id": "f234aae371d72680",
    "type": "function",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "parser",
    "func": "var json = msg.payload;\nvar subscriberCount = json.items[0].statistics.subscriberCount;\n\nmsg.payload = { \"text\": subscriberCount, \"icon\": 5029};\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "initialize": "",
    "finalize": "",
    "libs": [],
    "x": 590,
    "y": 120,
    "wires": [
      [
        "555bb8624b88c9c3"
      ]
    ]
  },
  {
    "id": "555bb8624b88c9c3",
    "type": "mqtt out",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "",
    "topic": "ulanzi/custom/youtube",
    "qos": "",
    "retain": "",
    "respTopic": "",
    "contentType": "",
    "userProps": "",
    "correl": "",
    "expiry": "",
    "broker": "346df2a95aac5785",
    "x": 800,
    "y": 120,
    "wires": []
  },
  {
    "id": "69c388146e28049d",
    "type": "comment",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "Youtube Follower",
    "info": "Just enter your channelID and Youtube API key in the \"Data\" node  and set your AWTRIX MQTT prefix.\nUses Icon 5029 (LM)",
    "x": 140,
    "y": 80,
    "wires": []
  },
  {
    "id": "a349ade5a57f7537",
    "type": "function",
    "z": "54b42d8d.cda474",
    "g": "2a59d30d07abe14f",
    "name": "Data",
    "func": "msg.payload = { \"id\": \"UCpGLALzRO0uaasWTsm9M99w\", \"key\": \"XXX\", \"part\":\"statistics\"}\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "initialize": "",
    "finalize": "",
    "libs": [],
    "x": 270,
    "y": 120,
    "wires": [
      [
        "dc7878f9.4756c8"
      ]
    ]
  },
  {
    "id": "346df2a95aac5785",
    "type": "mqtt-broker",
    "name": "",
    "broker": "localhost",
    "port": "1883",
    "clientid": "",
    "autoConnect": true,
    "usetls": false,
    "protocolVersion": "4",
    "keepalive": "60",
    "cleansession": true,
    "birthTopic": "",
    "birthQos": "0",
    "birthPayload": "",
    "birthMsg": {},
    "closeTopic": "",
    "closeQos": "0",
    "closePayload": "",
    "closeMsg": {},
    "willTopic": "",
    "willQos": "0",
    "willPayload": "",
    "willMsg": {},
    "userProps": "",
    "sessionExpiry": ""
  }
]  
  </code></pre>
</details>

This Node-RED flow retrieves and displays the subscriber count of a specified YouTube channel on an AWTRIX 3 device. The flow consists of the following nodes:

1. **Inject**: This node triggers the flow periodically (every hour) or manually.
2. **Data (Function)**: This node contains the YouTube channel ID and the YouTube API key. Replace "XXX" with your YouTube API key and Youtube ID. The node constructs a payload containing the channel ID, API key, and required statistics and sends it to the "HTTP request" node.
3. **HTTP request**: This node sends a GET request to the YouTube API to retrieve the channel's statistics. The response is returned as a JavaScript object and passed to the "parser" node.
4. **parser (Function)**: This node extracts the subscriber count from the received channel statistics and constructs a payload containing the count and an icon (Icon 5029). The payload is sent to the "MQTT out" node.
5. **MQTT out**: This node publishes the payload to the MQTT topic "ulanzi/custom/youtube" on a local MQTT broker. You also have to change the topic in this node to fit your mqtt prefix.
6. **Comment (Youtube Follower)**: This node contains additional information about the flow. It does not affect the flow's functionality.

To use this flow, replace the "XXX" in the "Data" node with your YouTube API key and ensure that the MQTT broker settings in the "MQTT out" node are correct.
The flow will then retrieve the subscriber count of the specified YouTube channel and display it on your AWTRIX device along with the icon.
This Flow uses icon 5029 from LM (Just download it from the awtrix webinterface). You can change the icon in the flow to your favorite one.



# MQTT Commands

### Switch to app  
**Topic**  
`[PREFIX]/switch`  
**Payload**  
`{"name":"time"}`

Build-in app names are
- `time`
- `date`
- `temp`
- `hum`
- `bat`

For custompages you need to call the name you set in the topic:
if 
`[PREFIX]/custom/test` is your topic, 
then `test` is the name
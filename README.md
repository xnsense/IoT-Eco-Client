# IoT-Eco-Client

Client library object for Arduino supporting MQTT messages and OTA updates in the IoT Ecosystem

This library is used on ESP8266 modules, in order to solve:
* Receiving commands as JSON formatted MQTT messages addressed to this unit
* Sending sensor data as JSON over MQTT
* Doing OTA updates when requested over MQTT

When initialized, the IoTEcoClient object will start listening on the MQTT server
on topic sensors/#

Any received message is parsed in the JSON library and it's tested for the presense of
a "id" propery matching this unit's MAC address.

Whenever such a message is received, it will be passed on to the registered callback function
(as a json object)

If the message says "Command"="UpgradeFromHttp", we'll look for an attribute "FirmwareUrl" and
an attempt to do an over-the-air (OTA) firmware update.

Example:
{
	"id":"5C:CF:7F:0D:A1:26",
	"Command":"UpgradeFromHttp",
	"FirmwareUrl":"http://mydomain.com/firmware/GarageDoor.1.0.0.10.bin"
}

At startup, the module will use a similar structure to report it's presense and data

At MQTT topic "sensors/GarageDoor/connected":
{ 
	"id": "5C:CF:7F:0D:A1:26", 
	"name": "GarageDoor", 
	"fw": "1.0.0.8" 
}

(Except the id, which is really the MAC address, these parameters are passed to the begin method)

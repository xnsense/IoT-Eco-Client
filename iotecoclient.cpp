// 
// 
// 

#include "iotecoclient.h"
IoTEcoClientClass::IoTEcoClientClass()
{

}
void IoTEcoClientClass::begin(String appName, const int version[], String ssid, String ssidPassword, String mqtt, int mqttPort, Stream& debugger)
{
	this->debugger = &debugger;
	begin(appName, version, ssid, ssidPassword, mqtt, mqttPort);
}

void IoTEcoClientClass::begin(String appName, const int version[], String ssid, String ssidPassword, String mqtt, int mqttPort)
{
	this->version = version;
	appName.toCharArray(this->appName, appName.length() + 1);
	ssid.toCharArray(this->ssid, ssid.length() + 1);
	ssidPassword.toCharArray(this->ssidPassword, ssidPassword.length() + 1);
	mqtt.toCharArray(this->mqttName, mqtt.length() + 1);
	this->mqttPort = mqttPort;

	String vMqttName = String(this->appName) + "[" + WiFi.macAddress() + "]";
	vMqttName.toCharArray(mqttClientName, vMqttName.length() + 1);

	this->printDeviceInfo();
	WiFi.begin(this->ssid, this->ssidPassword);
	this->mqtt.begin(mqttName, this->mqttPort, client);
	this->MQTT_connect();

	String topic = String("sensors/") + String(appName) + String("/connected");
	String message = String("{ \"id\": \"" + WiFi.macAddress() + "\", \"name\": \"" + appName + "\", \"fw\": \"" + GetVersionString() + "\" }");
	sendMqttMessage(topic, message);
}

void IoTEcoClientClass::loop()
{
	mqtt.loop();
	delay(10); // <- fixes some issues with WiFi stability

	if (!client.connected() || !mqtt.connected()) {
		MQTT_connect();
	}
}


void IoTEcoClientClass::sendMqttMessage(String message)
{
	String vTopic = String("sensors/") + String(appName);
	sendMqttMessage(vTopic, message);
}

void IoTEcoClientClass::sendMqttMessage(String topic, String message)
{
	if (!client.connected() || !mqtt.connected()) {
		MQTT_connect();
	}

	mqtt.publish(topic, message);
}


void IoTEcoClientClass::printDeviceInfo()
{
	if (debugger)
	{
		debugger->println("\r\n+++++++++++++++++++++++++++++++++++++\r\n");
		debugger->print("Device name : ");
		debugger->println(appName);
		debugger->print("MAC address: ");
		debugger->println(WiFi.macAddress());
		debugger->print("FW Version:  ");
		debugger->println(GetVersionString());
		debugger->println("\r\n-------------------------------------\r\n");
		debugger->println("Flash info:\r\n");
		printFlashInfo();
	}
}
void IoTEcoClientClass::printFlashInfo()
{
	if (debugger)
	{
		uint32_t realSize = ESP.getFlashChipRealSize();
		uint32_t ideSize = ESP.getFlashChipSize();
		FlashMode_t ideMode = ESP.getFlashChipMode();

		debugger->printf("Flash real id:   %08X\n", ESP.getFlashChipId());
		debugger->printf("Flash real size: %u\n\n", realSize);

		debugger->printf("Flash ide  size: %u\n", ideSize);
		debugger->printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
		debugger->printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

		if (ideSize != realSize) {
			debugger->println("Flash Chip configuration wrong!\n");
		}
		else {
			debugger->println("Flash Chip configuration ok.\n");
		}
	}
}




String IoTEcoClientClass::GetVersionString()
{
	String vVersion;
	for (int i = 0; i < 4; i++)
	{
		vVersion += version[i];
		if (i < 3)
			vVersion += ".";
	}
	return vVersion;
}



// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void IoTEcoClientClass::MQTT_connect() {
	// Connect to WiFi access point.
	if (debugger) debugger->println(); if (debugger) debugger->println();
	if (debugger) debugger->print("Connecting to ");
	if (debugger) debugger->println(this->ssid);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		if (debugger) debugger->print(".");
	}
	if (debugger) debugger->println();

	if (debugger) debugger->println("WiFi connected");
	if (debugger) debugger->println("IP address: ");
	if (debugger) debugger->println(WiFi.localIP());

	if (debugger) debugger->println("\nconnecting to MQTT...");

	while (!mqtt.connect(mqttClientName)) { //, "user", "pass")) {
		client.stop();
		mqtt.disconnect();
		if (debugger) debugger->print(".");
		delay(1000);
	}

	if (debugger) debugger->println("\nconnected to MQTT!");
	mqtt.subscribe("sensors/#");
}



void IoTEcoClientClass::UpgradeFirmware(String pUrl)
{
	char vUrlArray[255];
	pUrl.toCharArray(vUrlArray, pUrl.length() + 1);

	String message("");
	t_httpUpdate_return ret = ESPhttpUpdate.update(vUrlArray);

	switch (ret) {
	case HTTP_UPDATE_FAILED:
		message += "HTTP_UPDATE_FAILD Error (";
		message += ESPhttpUpdate.getLastError();
		message += "): ";
		message += ESPhttpUpdate.getLastErrorString().c_str();
		if (debugger) debugger->println(message);

		mqtt.publish("sensors/esp2", String("{\"id\": \"" + WiFi.macAddress() + "\", \"error\":\"" + message + "\"}"));
		break;

	case HTTP_UPDATE_NO_UPDATES:
		if (debugger) debugger->println("HTTP_UPDATE_NO_UPDATES");
		break;

	case HTTP_UPDATE_OK:
		ESP.restart();
		if (debugger) debugger->println("HTTP_UPDATE_OK");
		break;
	}
}

void IoTEcoClientClass::mqttMessageReceived(String topic, String payload, char * bytes, unsigned int length) {
	if (debugger)
	{
		debugger->print("incoming: ");
		debugger->print(topic);
		debugger->print(" - ");
		debugger->print(payload);
		debugger->println();
	}

	DynamicJsonBuffer jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(bytes);

	if (getJsonValue(json, "id").equals(WiFi.macAddress()))
	{
		if (getJsonValue(json, "Command").equals("UpgradeFromHttp"))
		{
			String vUrl = getJsonValue(json, "FirmwareUrl");
			if (vUrl.length() > 4)
			{
				if (debugger) debugger->print("Upgrade requested from url:");
				if (debugger) debugger->println(vUrl);
				UpgradeFirmware(vUrl);
			}
		}
		else
		{
			if (this->mqttMessageCallback)
				this->mqttMessageCallback(json);
		}
	}
}

String IoTEcoClientClass::getJsonValue(JsonObject& json, String key)
{
	for (JsonObject::iterator it = json.begin(); it != json.end(); ++it) {
		if (key.equals(it->key))
			return String(it->value.asString());
	}
	return String("");
}

void IoTEcoClientClass::setMqttMessageCallback(void(*mqttMessageCallback)(JsonObject& json))
{
	this->mqttMessageCallback = mqttMessageCallback;
}

IoTEcoClientClass IoTEcoClient;

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
	IoTEcoClient.mqttMessageReceived(topic, payload, bytes, length);
}


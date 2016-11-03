// 
// 
// 

void IoTEcoClientClass_mqttMessageReceived(char* topic, unsigned char* payload, unsigned int length);

#include "iotecoclient.h"
IoTEcoClientClass::IoTEcoClientClass()
{

}

void IoTEcoClientClass::beginSecure(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort, const char* mqttClientID, const char* mqttUser, const char* mqttPass, const char* mqttPublishTopic, const char* mqttSubscribeTopic, Stream& debugger)
{
	this->mqttClientID = mqttClientID;
	this->mqttPublishTopic = mqttPublishTopic;
	this->mqttSubscribeTopic = mqttSubscribeTopic;

	this->secure = true;
	this->debugger = &debugger;
	begin(appName, version, ssid, ssidPassword, mqtt, mqttPort, mqttUser, mqttPass);
}
void IoTEcoClientClass::begin(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort, const char* mqttUser, const char* mqttPass, Stream& debugger)
{
	this->debugger = &debugger;
	begin(appName, version, ssid, ssidPassword, mqtt, mqttPort, mqttUser, mqttPass);
}
void IoTEcoClientClass::begin(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort, const char* mqttUser, const char* mqttPass)
{
	this->mqttUser = mqttUser;
	this->mqttPass = mqttPass;
	begin(appName, version, ssid, ssidPassword, mqtt, mqttPort);
}
void IoTEcoClientClass::begin(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort, Stream& debugger)
{
	this->debugger = &debugger;
	begin(appName, version, ssid, ssidPassword, mqtt, mqttPort);
}

void IoTEcoClientClass::begin(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort)
{
	this->version = version;
	this->appName = appName;
	this->ssid = ssid;
	this->ssidPassword = ssidPassword;
	
	if (secure)
		client = new WiFiClientSecure();
	else
		client = new WiFiClient();

	this->mqttName = mqtt;
	this->mqttPort = mqttPort;

	String vMqttName = String(this->appName) + "[" + WiFi.macAddress() + "]";
	mqttClientName = mqttClientID == 0 ? vMqttName.c_str() : mqttClientID;
	
	this->mqtt = PubSubClient(*client);

	this->printDeviceInfo();
	WiFi.begin(this->ssid, this->ssidPassword);
	this->mqtt.setServer(mqttName, this->mqttPort);

	//std::function<void(char*, unsigned char*, unsigned int)> vCallback = MakeDelegate(this, IoTEcoClientClass::mqttMessageReceived);
	this->mqtt.setCallback(IoTEcoClientClass_mqttMessageReceived);
	this->MQTT_connect();

	String topic = mqttPublishTopic == 0 ? String("sensors/") + String(appName) + String("/connected") : String(mqttPublishTopic);
	String message = String("{ \"id\": \"" + WiFi.macAddress() + "\", \"name\": \"" + appName + "\", \"fw\": \"" + GetVersionString() + "\" }");
	sendMqttMessage(topic, message);
}

void IoTEcoClientClass::loop()
{
	mqtt.loop();
	delay(10); // <- fixes some issues with WiFi stability

	if (!client->connected() || !mqtt.connected()) {
		MQTT_connect();
	}
}

bool IoTEcoClientClass::connected()
{
	return client->connected() && mqtt.connected();
}

void IoTEcoClientClass::sendMqttMessage(String message)
{
	String vTopic = mqttPublishTopic == 0 ? String("sensors/") + String(appName) : String(mqttPublishTopic);
	sendMqttMessage(vTopic, message);
}

void IoTEcoClientClass::sendMqttMessage(String topic, String message)
{
	if (!client->connected() || !mqtt.connected()) {
		MQTT_connect();
	}

	mqtt.publish(topic.c_str(), message.c_str());
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

	while (!mqtt.connected()) { // (!mqtt.connect(mqttClientName)) { //, "user", "pass")) {
		if ((mqttUser == 0 && mqtt.connect(mqttClientName)) || (mqttUser != 0 && mqtt.connect(mqttClientName, mqttUser, mqttPass)))
		{
			if (debugger) debugger->println("\nSuccessfully connected to MQTT!");
			const char * vTopic = mqttSubscribeTopic == 0 ? "sensors/#" : mqttSubscribeTopic;
			mqtt.subscribe(vTopic);
		}
		else
		{
			if (debugger) debugger->print(".");
			if (debugger) debugger->print("failed, rc=");
			if (debugger) debugger->print(mqtt.state());
			if (debugger) debugger->println(" try again in 5 seconds");
			// Wait 2 seconds before retrying
			delay(2000);
		}
	}

	
}



void IoTEcoClientClass::UpgradeFirmware(String pUrl)
{
	String initMessage = String("{ \"id\": \"" + WiFi.macAddress() + "\", \"fw\": \"" + GetVersionString() + "\", \"message\":\"OTA upgrade from [" + pUrl + "]\" }");
	sendMqttMessage(initMessage);

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

		mqtt.publish("sensors/esp2", String("{\"id\": \"" + WiFi.macAddress() + "\", \"error\":\"" + message + "\"}").c_str());
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

void IoTEcoClientClass::mqttMessageReceived(char* topic, unsigned char* payload, unsigned int length) {
	
	char message[MQTT_MAX_PACKET_SIZE];
	for (int i = 0; i < length; i++)
		message[i] = payload[i];
	message[length] = 0;
	
	if (debugger)
	{
		debugger->print("incoming: ");
		debugger->print(topic);
		debugger->print(" - ");
		debugger->print(message);
		debugger->println();
	}

	DynamicJsonBuffer jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(message);

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
			{
				if (debugger) debugger->println("Passing message on to client");
				this->mqttMessageCallback(json);
			}
			else
			{
				if (debugger) debugger->println("Client is not subscribing to messages");
			}
		}
	}
	else
	{
		if (debugger) debugger->println("Message was not addressed for this device");
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



void IoTEcoClientClass_mqttMessageReceived(char* topic, unsigned char* payload, unsigned int length) {
	IoTEcoClient.mqttMessageReceived(topic, payload, length);
}

// xinit.h

#ifndef _IOTECOCLIENTCLASS_h
#define _IOTECOCLIENTCLASS_h


#ifndef WIFI_CONNECTION_TIMEOUT
#define WIFI_CONNECTION_TIMEOUT 30000
#endif // !WIFI_CONNECTION_TIMEOUT

#include <arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include "WebConfig.h"

class IoTEcoClientClass
{
private:
	bool secure = false;
	Stream* debugger = NULL;
	const char* appName;
	const int* version;
	const char* ssid;
	const char* ssidPassword;
	const char* mqttName;
	int mqttPort;
	const char* mqttUser = 0;
	const char* mqttPass = 0;

	const char* mqttClientName = 0;
	const char* mqttClientID = 0;
	const char* mqttPublishTopic = 0;
	const char* mqttSubscribeTopic = 0;
	unsigned long aliveMessageInterval = 0;
	unsigned long lastAliveMessage = 0;

	int accessPointButtonPin = -1;

	WiFiClient *client;
	byte mac[6];                     // the MAC address of your Wifi shield
	PubSubClient mqtt;

	void MQTT_connect();
	void UpgradeFirmware(String pUrl);

	void(*mqttMessageCallback)(JsonObject& json) = NULL;

	WebConfigClass *webConfig = 0;

	bool isAPButtonPressed();

protected:

public:
	IoTEcoClientClass();
	void begin(const char* appName, const int version[], int accessPointButtonPin, Stream& debugger);
	void begin(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort);
	void begin(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort, Stream& debugger);
	void begin(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort, const char* mqttUser, const char* mqttPass);
	void begin(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort, const char* mqttUser, const char* mqttPass, Stream& debugger);
	void beginSecure(const char* appName, const int version[], const char* ssid, const char* ssidPassword, const char* mqtt, int mqttPort, const char* mqttClientID, const char* mqttUser, const char* mqttPass, const char* mqttPublishTopic, const char* mqttSubscribeTopic, Stream& debugger);
	void loop();
	bool connected();
	void printFlashInfo();
	void printDeviceInfo();
	String GetVersionString();
	
	void sendMqttMessage(String message);
	void sendMqttMessage(JsonObject& message);
	void sendMqttData(String message);
	void sendMqttData(JsonObject& message);

	void setMqttMessageCallback(void(*mqttMessageCallback)(JsonObject& json));
	void sendConfigMessage();
	void setConfig(JsonObject& json);
	bool hasValue(JsonObject& json, String key);
	String getJsonValue(JsonObject& json, String key);
	JsonVariant getJsonObject(JsonObject& json, String key);
	void mqttMessageReceived(char* topic, unsigned char* payload, unsigned int length);
	void setAliveMessageInterval(unsigned long interval);
};

extern IoTEcoClientClass IoTEcoClient;


#endif


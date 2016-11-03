// xinit.h

#ifndef _IOTECOCLIENTCLASS_h
#define _IOTECOCLIENTCLASS_h

#include "arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

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

	WiFiClient *client;
	byte mac[6];                     // the MAC address of your Wifi shield
	PubSubClient mqtt;

	void MQTT_connect();
	void UpgradeFirmware(String pUrl);

	void(*mqttMessageCallback)(JsonObject& json) = NULL;
protected:


public:
	IoTEcoClientClass();
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
	void sendMqttMessage(String topic, String message);
	void sendMqttMessage(JsonObject& message);
	void sendMqttMessage(String topic, JsonObject& message);
	void setMqttMessageCallback(void(*mqttMessageCallback)(JsonObject& json));
	String getJsonValue(JsonObject& json, String key);
	void mqttMessageReceived(char* topic, unsigned char* payload, unsigned int length);
};

extern IoTEcoClientClass IoTEcoClient;


#endif


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
	char appName[30];
	const int* version;
	char ssid[30];
	char ssidPassword[30];
	char mqttName[30];
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
	void begin(String appName, const int version[], String ssid, String ssidPassword, String mqtt, int mqttPort);
	void begin(String appName, const int version[], String ssid, String ssidPassword, String mqtt, int mqttPort, Stream& debugger);
	void begin(String appName, const int version[], String ssid, String ssidPassword, String mqtt, int mqttPort, String mqttUser, String mqttPass);
	void begin(String appName, const int version[], String ssid, String ssidPassword, String mqtt, int mqttPort, String mqttUser, String mqttPass, Stream& debugger);
	void beginSecure(String appName, const int version[], String ssid, String ssidPassword, String mqtt, int mqttPort, String mqttClientID, String mqttUser, String mqttPass, String mqttPublishTopic, String mqttSubscribeTopic, Stream& debugger);
	void loop();
	void printFlashInfo();
	void printDeviceInfo();
	String GetVersionString();
	void sendMqttMessage(String message);
	void sendMqttMessage(String topic, String message);
	void setMqttMessageCallback(void(*mqttMessageCallback)(JsonObject& json));
	String getJsonValue(JsonObject& json, String key);
	void mqttMessageReceived(char* topic, unsigned char* payload, unsigned int length);
};

extern IoTEcoClientClass IoTEcoClient;


#endif


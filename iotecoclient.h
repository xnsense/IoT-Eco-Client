// xinit.h

#ifndef _XINIT_h
#define _XINIT_h

#include "arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>
#include <MQTTClient.h>

class IoTEcoClientClass
{
private:
	Stream* debugger = NULL;
	char appName[30];
	const int* version;
	char ssid[30];
	char ssidPassword[30];
	char mqttName[30];
	int mqttPort;

	char mqttClientName[60];

	WiFiClient client;
	byte mac[6];                     // the MAC address of your Wifi shield
	MQTTClient mqtt;

	void MQTT_connect();
	void UpgradeFirmware(String pUrl);
	String getJsonValue(JsonObject& json, String key);

	void(*mqttMessageCallback)(JsonObject& json) = NULL;
protected:


public:
	IoTEcoClientClass();
	void begin(String appName, const int version[], String ssid, String ssidPassword, String mqtt, int mqttPort);
	void begin(String appName, const int version[], String ssid, String ssidPassword, String mqtt, int mqttPort, Stream& debugger);
	void loop();
	void printFlashInfo();
	void printDeviceInfo();
	String GetVersionString();
	void sendMqttMessage(String message);
	void sendMqttMessage(String topic, String message);
	void mqttMessageReceived(String topic, String payload, char * bytes, unsigned int length);
	void setMqttMessageCallback(void(*mqttMessageCallback)(JsonObject& json));
};

extern IoTEcoClientClass IoTEcoClient;


#endif


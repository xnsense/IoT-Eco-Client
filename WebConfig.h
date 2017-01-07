// WebConfig.h

#ifndef _WEBCONFIG_h
#define _WEBCONFIG_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Config.h"

class WebConfigClass
{
public:
	WebConfigClass()
	{
		
	}
	static void Setup(Config *pConfig);
	static void Handle();
	static Config *config;

protected:
	static void SetupWebServer();
	static void WebServer_GetConfig();
	static void WebServer_SaveConfig();

private:
	static const char* AP_SSID;
	static ESP8266WebServer gWebServer;
	static String getConfigPage();
	static String saveConfigResponsePage();
};


#endif

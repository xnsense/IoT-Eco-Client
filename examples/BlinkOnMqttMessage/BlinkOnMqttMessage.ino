#include <iotecoclient.h>

#define APP_NAME    "mySEnsorApp"
#define WLAN_SSID       "myAccessPoint"
#define WLAN_PASS       "myAPPassword"
#define AIO_SERVER      "myMQTTAddress"
#define AIO_SERVERPORT  1883
const int version[] = { 1, 0, 0, 0 };


#define PIN_LED 5
bool isLedOn = false;

void setup() {

  Serial.begin(9600);
  while (!Serial);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, isLedOn);
  
  IoTEcoClient.begin(APP_NAME, version, WLAN_SSID, WLAN_PASS, AIO_SERVER, AIO_SERVERPORT, Serial);
  IoTEcoClient.setMqttMessageCallback(mqttMessageReceived);
  
  Serial.println("Setup complete");
}


void loop() {
  IoTEcoClient.loop();
  // do any other business
}


void mqttMessageReceived(JsonObject& json)
{
  String vJson("");
  json.prettyPrintTo(vJson);
  Serial.println("Received JSON on MQTT:");
  Serial.println(vJson);

  isLedOn = !isLedOn;
  digitalWrite(PIN_LED, isLedOn);
}


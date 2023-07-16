#ifndef mqtt_h
#define mqtt_h

#include "EspMQTTClient.h"



void mqttInit();

#define WIFINAME "wifi"
#define WIFIPASSWORD "1234567890"
#define IP "192.168.224.243"
#define PORT 1883
#define CLIENTNAME "DATALOGGER"


void mqttSendMessage(String message);
void mqttLoop();

#endif
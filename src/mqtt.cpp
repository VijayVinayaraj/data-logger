#include "mqtt.h"


EspMQTTClient client(
  WIFINAME,
  WIFIPASSWORD,
  IP,  // MQTT Broker server ip
  CLIENTNAME,     // Client name that uniquely identify your device
  PORT              // The MQTT port, default to 1883. this line can be omitted
);


void mqttInit(){
   client.enableDebuggingMessages();
   void enableMQTTPersistence();
}


void onConnectionEstablished()
{
  



}

void mqttSendMessage(String message){

if(client.isMqttConnected()){

client.publish("topic/data",message);
    delay(800);
}

}

void mqttLoop(){

    client.loop();
}
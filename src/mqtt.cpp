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
  // Subscribe to "mytopic/test" and display received message to Serial
 

  // Publish a message to "mytopic/test"
  client.publish("mytopic/test", "This is a message"); // You can activate the retain flag by setting the third parameter to true

  client.executeDelayed(5 * 1000, []() {
    client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  });



}

void mqttSendMessage(){
String message = "messga"+78;

if(client.isMqttConnected()){

client.publish("topic/data",message);
    delay(1000);
}

}

void mqttLoop(){
mqttSendMessage();
    client.loop();
}
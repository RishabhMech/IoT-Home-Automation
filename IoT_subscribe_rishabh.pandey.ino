
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define ldr A0

#define wifi "DSX100_5G"    //declare available wifi details
#define password "DSX690893"

#define server "io.adafruit.com"
#define port 1883    //use 8883 for SSL security
#define username "rishabhada"
#define key "dbce04865bd04efc81e5574337f98dbf"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, server, port, username, key);
Adafruit_MQTT_Subscribe feed = Adafruit_MQTT_Subscribe(&mqtt, username"/feeds/rishabh12"); //declare feed path

int led = 16;

//Code to subscribe the feed data

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(115200);   //declare the baud rate
  delay(100);

  //start connecting the client (node MCU) to the WiFi and MQTT
  Serial.println("connecting to ");
  Serial.println(wifi);
  WiFi.begin(wifi, password);   //starts the procedure to connect client to the WiFi

  //till the wifi connects, print …….
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected to client");
  Serial.println("IP address is ");
  Serial.print(WiFi.localIP());   //IP address

  mqtt.subscribe(&feed);    //subscribe to feed from adafruit

  Serial.println("connecting to mqtt");
  while (mqtt.connect())  //till the mqtt connects, print …….
  {
    delay(500);
    Serial.println(".");
  }
  Serial.println("mqtt connected to ecosystem");
}

void loop()   // Start the task
{
  while (mqtt.readSubscription(5000))
  {
    //Print the new value to the serial monitor
    Serial.print("feed: ");
    Serial.println((char*)feed.lastread);

    //If the feed signal is "ON" turn the light on, otherwise, turn it off!
    if (!strcmp((char*)feed.lastread, "ON"))
    {
      digitalWrite(led, LOW);
    }
    else
      digitalWrite(led, HIGH);
  }
}

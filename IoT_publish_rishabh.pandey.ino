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
Adafruit_MQTT_Publish feed = Adafruit_MQTT_Publish(&mqtt, username"/feeds/rishabh12"); //declare feed path

int led = 16;

//Code to publish the feed data

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
  if (mqtt.connected())
  {
    int data = analogRead(ldr);
    Serial.print("\nSending ldr value");
    Serial.print(data);
    Serial.print("……");
    if (feed.publish(data))
    {
      Serial.println("Success");
    }
    else
    {
      Serial.println("Fail");
    }
  }
  delay(800);
}

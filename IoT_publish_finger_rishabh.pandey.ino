#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);

#include "Adafruit_Fingerprint.h"
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

#define ldr A0

#define wifi "DSX100_5G"    //declare available wifi details
#define password "DSX690893"

#define server "io.adafruit.com"
#define port 1883    // for SSL encrypted connection security
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

  Serial.begin(115200);   //set the data transfer rate
  delay(100);

  while (!Serial);
  delay(100);
  Serial.println("Serial avialble!");

  //Fingerprint sensor module setup
  finger.begin(57600);   // set the data transfer rate for the sensor serial port

  while (!finger.verifyPassword())
  {
    delay(10);
  }
  Serial.println("Found fingerprint sensor!");

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  Serial.println("Ready to enroll a fingerprint?");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as!");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
    return;
  }
  Serial.print("Enrolling ID #"); Serial.println(id);
  while (!getFingerprintEnroll());

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

uint8_t readnumber(void) //method 1
{
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

uint8_t getFingerprintEnroll() //method 2
{

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
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

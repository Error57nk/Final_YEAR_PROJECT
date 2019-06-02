
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID       "SEN VILLA"
#define WLAN_PASS       "asdfghjkl"



#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Ecesit2019"
#define AIO_KEY         "afc6a567eda94a9fb6663a95fe1d827d"


#define led D2
#define bld D3

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe int1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ats");

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(led, OUTPUT);
  Serial.println(F("SETUP OF Reciver END"));
  pinMode(bld, OUTPUT);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqtt.subscribe(&int1);
}

void loop() {
  String state1;
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    if (subscription == &int1)
    {
      Serial.print(F("Got: "));
      Serial.println((char *)int1.lastread);
      state1 = (char *)int1.lastread;
      Serial.println(state1);
    }
  }
  if (state1 == "5")
  {
    Serial.println("state1");
    digitalWrite(D2, HIGH);
    digitalWrite(D3, LOW);
  }
  else if (state1 == "0")
  {
    Serial.println("STATE0");
    digitalWrite(bld, HIGH);
    digitalWrite(D2, LOW);
  }
}




// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}


#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "SEN VILLA"
#define WLAN_PASS       "asdfghjkl"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Ecesit2019"
#define AIO_KEY         "afc6a567eda94a9fb6663a95fe1d827d"
/************ Global State (you don't need to change this!) ******************/
volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned int flowsensor =D3  ; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
#define trigPin D1 // define the pins of your sensor
#define echoPin D2 
void flow () // Interrupt function
{
   flow_frequency++;
}


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Adaset = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/waterf");
Adafruit_MQTT_Publish Adase = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ussit");
Adafruit_MQTT_Publish Adas = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ats");

void MQTT_connect();

void setup() {
  Serial.begin(9600);

   pinMode(trigPin, OUTPUT);// set the trig pin to output (Send sound waves)
  pinMode(echoPin, INPUT);

  delay(10);
  pinMode(flowsensor, INPUT);
   digitalWrite(flowsensor, HIGH);
 
  Serial.println(F("***Starting The Server***"));
  attachInterrupt(0, flow, RISING); // Setup Interrupt
   sei(); // Enable interrupts
   currentTime = millis();
   cloopTime = currentTime;
 
  
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
  }

uint32_t x=0;
uint32_t fs=0;
uint32_t us=0;
uint32_t at=0;





void loop() {
 
  MQTT_connect();
  fs=Flow_Sen();
  delay(1000);
  us=Ultra_Sen();
  if(us<=3 && fs >= 0)
  {
    at=5;
  }
  else{
    at=0;
  }
  

  ///////YOUR Condition Here*************************************************/////////

if (! Adaset.publish(fs)) {
    Serial.println(F("Failed_Not send"));
  } else {
    Serial.println(F("fs Output Updated OK!"));
  } 
    delay(3000);  

if (! Adase.publish(us)) {
    Serial.println(F("Failed_Not send"));
  } else {
    Serial.println(F("Us Output Updated OK!"));
  } 
    delay(3000);  

if (! Adas.publish(at)) {
    Serial.println(F("Failed_Not send"));
  } else {
    Serial.println(F("alert  Updated OK!"));
  } 
    delay(3000);  
  Serial.println(at);
}


void MQTT_connect() {
  int8_t ret;


  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
        
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}


int Flow_Sen()
{       
        currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      flow_frequency = 0; // Reset Counter
      Serial.print(l_hour, DEC); // Print litres/hour
      Serial.println(" L/hour");
      return(l_hour);
   }
}

int Ultra_Sen()
{
  long duration, distance; 
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); // delays are required for a succesful sensor operation.
  digitalWrite(trigPin, HIGH);

  delayMicroseconds(10); //this delay is required as well!
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  return(distance);
}

/*
 * mqtt.cpp
 *
 *  Created on: Jun 5, 2017
 *      Author: ipaev
 */

#include "mqtt.hpp"


#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <PubSubClient.h>
static WiFiClient espClient;
static PubSubClient client(espClient);
static const char* mqtt_server = "broker.mqttdashboard.com";
static const byte ledPin = 13; // Pin with LED

void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  Serial.print(receivedChar);
  if (receivedChar == '0')
  // ESP8266 Huzzah outputs are "reversed"
  digitalWrite(ledPin, HIGH);
  if (receivedChar == '1')
   digitalWrite(ledPin, LOW);
  }
  Serial.println();
}


void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP8266 Client")) {
  Serial.println("connected");
  // ... and subscribe to topic
  client.subscribe("ledStatus");
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}

PubSubClient * setup_mqtt() {
	  //WiFiManager
	  //Local intialization. Once its business is done, there is no need to keep it around
	  WiFiManager wifiManager;

	  String ssid = "THERM" + String(ESP.getChipId());
	  delay(3000);
	  if (!wifiManager.autoConnect(ssid.c_str())) {
	    Serial.println("failed to connect and hit timeout");
	    delay(3000);
	    //reset and try again, or maybe put it to deep sleep
	    ESP.reset();
	    delay(5000);
	  }

	  //if you get here you have connected to the WiFi
	  Serial.println("connected...yeey :)");

	  //mqtt server start
	  client.setServer(mqtt_server, 1883);
	  client.setCallback(callback);
	  pinMode(ledPin, OUTPUT);
	  //mqtt server end
	  return &client;
}
int loop_mqtt() {
	 if (!client.connected()) {
	  reconnect();
	 }
	 client.loop();

	return 0;
}

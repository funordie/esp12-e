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

#include <PubSubClient.h>
static WiFiClient espClient;
static PubSubClient client(espClient);
static const char* mqtt_server = "broker.mqttdashboard.com";
static const byte ledPin = 5; // Pin with LED
uint32_t chip_id;

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (unsigned int i=0;i<length;i++) {
		char receivedChar = (char)payload[i];
		Serial.print(receivedChar);
		if (receivedChar == '0')
			digitalWrite(ledPin, LOW);
		if (receivedChar == '1')
			digitalWrite(ledPin, HIGH);
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
			String str = String(chip_id) + "_" + "ledStatus";
			Serial.println("subscribe to:" + str);
			client.subscribe(str.c_str());
		} else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

void setup_mqtt() {
	EspClass esp;
	chip_id = esp.getChipId();

	//mqtt server start
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
	pinMode(ledPin, OUTPUT);
	//mqtt server end
}
int loop_mqtt() {
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	return 0;
}

void publish_temperature_mqtt(float temp) {
	String str = String(chip_id) + "_" + "temperature";
	Serial.println("publish:" + str);
	client.publish(str.c_str(), String(temp).c_str(), true);
}

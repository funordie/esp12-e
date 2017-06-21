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
static uint32_t chip_id;

static String topic_OnTime;
static String topic_Start;

extern "C" {
#include "user_interface.h"
}

#define ONCE 0
#define REPEAT 1

static os_timer_t delay_timer;

void delay_end(void* arg) {
	(void) arg;
	digitalWrite(ledPin, LOW);
}

void start_OnTime_Period(unsigned long ms) {
	Serial.println("Start OnTime:" + String(ms));
	os_timer_disarm(&delay_timer);
    os_timer_arm(&delay_timer, ms, ONCE);
    digitalWrite(ledPin, HIGH);
}
void callback(char* topic, byte* payload, unsigned int length) {

	static uint32_t OnTime;

	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");

	if(!strcmp(topic_OnTime.c_str(), topic)) {
		//received topic OnTime
		String str;
		for (unsigned int i=0;i<length;i++) {
			str += (char)payload[i];
		}
		Serial.println(str);
		OnTime = str.toInt();
	}
	else if(!strcmp(topic_Start.c_str(), topic)) {
		//received topic LedStatus
		for (unsigned int i=0;i<length;i++) {
			char receivedChar = (char)payload[i];
			Serial.print(receivedChar);
			if (receivedChar == '0')
				digitalWrite(ledPin, LOW);
			if (receivedChar == '1') {
				if(OnTime) {
					start_OnTime_Period(OnTime);
				}
			}
		}
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

			Serial.println("subscribe to:" + topic_Start);
			client.subscribe(topic_Start.c_str());

			Serial.println("subscribe to:" + topic_OnTime);
			client.subscribe(topic_OnTime.c_str());
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

	topic_Start = String(chip_id) + "_" + "Start";
	topic_OnTime = String(chip_id) + "_" + "OnTime";

	//mqtt server start
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
	pinMode(ledPin, OUTPUT);
	//mqtt server end

    os_timer_setfn(&delay_timer, (os_timer_func_t*) &delay_end, 0);
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

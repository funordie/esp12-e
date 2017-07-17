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

//static const char* mqtt_server = "test.mosquitto.org";
//static const char* mqtt_server = "broker.hivemq.com";
static const char* mqtt_server = "iot.eclipse.org";
static const uint16_t mqtt_port = 1883;

static const byte pumpPin = 5; // Pin with LED
static const uint8_t statusPin = 14;
static String chip_id_str;
#define STR_ID_BASE 16

static String topic_OnTime;
static String topic_Start;
static String topic_CommandCB;

extern "C" {
#include "user_interface.h"
}

#define ONCE 0
#define REPEAT 1

static os_timer_t delay_timer;
volatile uint8_t ontime_running = 0;

static void mqtt_subscribe() {
	Serial.println("subscribe to:" + topic_OnTime);
	client.subscribe(topic_OnTime.c_str());

	Serial.println("subscribe to:" + topic_Start);
	client.subscribe(topic_Start.c_str());

	Serial.println("subscribe to:" + topic_CommandCB);
	client.subscribe(topic_CommandCB.c_str());
}

static void mqtt_unsubscribe() {
	Serial.println("unsubscribe to:" + topic_Start);
	client.unsubscribe(topic_Start.c_str());

	Serial.println("unsubscribe to:" + topic_OnTime);
	client.unsubscribe(topic_OnTime.c_str());

	Serial.println("unsubscribe to:" + topic_CommandCB);
	client.unsubscribe(topic_CommandCB.c_str());
}
void delay_end(void* arg) {
	(void) arg;
	ontime_running = 0;
	digitalWrite(pumpPin, LOW);
}

void start_OnTime_Period(unsigned long ms) {
	static uint32_t count = 0;
	ontime_running = 1;
	Serial.println("Start pump with time:" + String(ms) + "[ms]");
	os_timer_disarm(&delay_timer);
    os_timer_arm(&delay_timer, ms, ONCE);
    digitalWrite(pumpPin, HIGH);

    //TODO: save count values between resets
	String msg = "OnTime:" + String(ms) + " count:" + String(++count);
	Serial.println("publish: " + topic_CommandCB + " val:" + msg);
	client.publish(topic_CommandCB.c_str(), msg.c_str() , true);
}
void callback(char* topic, byte* payload, unsigned int length) {

	static uint32_t OnTime, Start;

	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] value: ");

	String str;
	for (unsigned int i=0;i<length;i++) {
		str += (char)payload[i];
	}
	Serial.println(str);

	if(!strcmp(topic_OnTime.c_str(), topic)) {
		//received topic OnTime
		OnTime = str.toInt();
	}
	else if(!strcmp(topic_Start.c_str(), topic)) {
		//received topic LedStatus
		if(str.toInt() == 1) {
			Start = 1;
		}
		else {
			Start = 0;
		}
	}
//    else if(!strcmp(topic_CommandCB.c_str(), topic)) {
//    	//received topic CommandCB
//    	const char * ontime_str = strstr(str.c_str(), "OnTime:");
//    	const char * count_str = strstr(str.c_str(), "count:");
//    	Serial.println("found string OnTime:" + String(ontime_str) + " count:" + String(count_str));
//    }

	//process command
	if(Start == 1 && OnTime > 0) {
		start_OnTime_Period(OnTime);
		client.publish(topic_Start.c_str(), "0", true);
	}
	Serial.println("receive end");
}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect("ESP8266 Client")) {
			Serial.println("connected");

			mqtt_subscribe();
		} else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

void setup_mqtt(uint32_t chip_id) {
	chip_id_str = String(chip_id, STR_ID_BASE);
	chip_id_str.toUpperCase();

	printf("%s: %s", __FUNCTION__, chip_id_str.c_str());
	topic_Start = chip_id_str + "_" + "Start";
	topic_OnTime = chip_id_str + "_" + "OnTime";
	topic_CommandCB = chip_id_str + "_" + "CommandCB";

	//mqtt server start
	client.setServer(mqtt_server, mqtt_port);
	client.setCallback(callback);

	mqtt_subscribe();

	pinMode(pumpPin, OUTPUT);
	pinMode(statusPin, OUTPUT);
	//mqtt server end

    os_timer_setfn(&delay_timer, (os_timer_func_t*) &delay_end, 0);
}
int loop_mqtt() {
	if (!client.connected()) {
		digitalWrite(statusPin, LOW);
		reconnect();
	}
	digitalWrite(statusPin, HIGH);
	client.loop();

	return 0;
}

void publish_temperature_mqtt(float temp) {
	String str = chip_id_str + "_" + "Temperature";
//	Serial.println("publish:" + str);
	client.publish(str.c_str(), String(temp).c_str(), true);
}

void publish_moisture_mqtt(uint16_t mst) {
	String str = chip_id_str + "_" + "Moisture";
//	Serial.println("publish:" + str);
	client.publish(str.c_str(), String(mst).c_str(), true);
}

void mqtt_deinit() {

    mqtt_unsubscribe();

	Serial.println("mqtt: disconnect:");
	client.disconnect();
}

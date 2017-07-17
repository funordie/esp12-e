/*
 * wifi.cpp
 *
 *  Created on: Jun 8, 2017
 *      Author: ipaev
 */
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "common.h"

void setup_wifi() {
	//WiFiManager
	//Local intialization. Once its business is done, there is no need to keep it around
	WiFiManager wifiManager;

	String ssid = "THERM" + String(ESP.getChipId());
	delay(3000);
	if (!wifiManager.autoConnect(ssid.c_str())) {
		Serial.println("failed to connect and hit timeout");
#if 0
		delay(3000);
		//reset and try again, or maybe put it to deep sleep
		ESP.reset();
#else
		Serial.println("deep sleep: start");
		ESP.deepSleep(SLEEPING_TIME_IN_SECONDS * 1000*1000, WAKE_RF_DEFAULT);
#endif
		delay(5000);
	}

	//if you get here you have connected to the WiFi
	Serial.println("connected...yeey :)");
}

void wifi_deinit() {

}

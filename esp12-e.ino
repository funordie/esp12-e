#include "ds18b20.hpp"
#include "mqtt.hpp"
#include "wifi.hpp"
#include "moisture.hpp"

#include "common.h"

#include <SPI.h>
#include <SD.h>
const int chipSelect = 10;
static float temperature;
static uint16_t moisture;

extern volatile uint8_t ontime_running;

void setup() {

	uint32_t chip_id = ESP.getChipId();

	// put your setup code here, to run once:
	Serial.begin(115200);
	Serial.println("\n Starting...");

	// see if the card is present and can be initialized:
	if (!SD.begin(chipSelect)) {
		Serial.println("Card failed, or not present");
	}

	setup_wifi();
	setup_mqtt(chip_id);
	setup_temperature();
    setup_moisture();

	Serial.println("exit setup");
}

void loop() {
	// put your main code here, to run repeatedly:
	loop_mqtt();

    if(!loop_moisture(&moisture)) {
    	//publish moisture
    	publish_moisture_mqtt(moisture);
    }

	if(!loop_temperature(&temperature)) {
		//valid temperature

		publish_temperature_mqtt(temperature);

		//write temperature
		// open the file. note that only one file can be open at a time,
		// so you have to close this one before opening another.
		File dataFile = SD.open("datalog.txt", FILE_WRITE);
		// if the file is available, write to it:
		if (dataFile) {
			String dataString = "";
			dataString += String(temperature);
			dataFile.println(dataString);
			dataFile.close();
			// print to the serial port too:
			Serial.println(dataString);
		}
		// if the file isn't open, pop up an error:
		else {
//			Serial.println("error opening datalog.txt");
		}
	}

	static uint32_t count = 0;
	if(count >= 10 && ontime_running == 0) {
		mqtt_deinit();
		wifi_deinit();

		Serial.println("deep sleep: start");
		ESP.deepSleep(SLEEPING_TIME_IN_SECONDS * 1000*1000, WAKE_RF_DEFAULT);
		delay(500); // wait for deep sleep to happen
	}
	count++;
}

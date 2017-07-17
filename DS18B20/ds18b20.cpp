#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 4
#define DS18B20_UPDATE_TIME 5000
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup_temperature(void) {
	// Start up the library
	sensors.begin();
}

int loop_temperature(float * temp) {

	static uint8_t init = 0;
	static long nowPrev = 0;
	long now = millis();

	if(init && ((now - nowPrev) < DS18B20_UPDATE_TIME)) return -1;

	sensors.requestTemperatures(); // Send the command to get temperature
	float res = sensors.getTempCByIndex(0);
	if(res == DEVICE_DISCONNECTED_C) {
		return -1;
	}
	else {
		init = 1;
		nowPrev = now;
	}

	*temp = res;
//	printf("Temperature:%f\n", *temp);
	return 0;
}

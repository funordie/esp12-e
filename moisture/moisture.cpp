/*
 * moisture.cpp
 *
 *  Created on: Jun 27, 2017
 *      Author: ipaev
 */
#include <stdint.h>
#include <Arduino.h>
#include "moisture.hpp"

#define PROBE_PIN A0
#define MOISTURE_UPDATE_TIME 5000

void setup_moisture() {

}

int loop_moisture(uint16_t * mst) {
	static long nowPrev = 0;
	long now = millis();
	if((now - nowPrev) < MOISTURE_UPDATE_TIME) return -1;
	nowPrev = now;

	*mst = (uint16_t)analogRead(PROBE_PIN);
    return 0;
}


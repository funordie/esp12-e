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

	static uint8_t init = 0;
	static long nowPrev = 0;
	long now = millis();
	if(init && ((now - nowPrev) < MOISTURE_UPDATE_TIME)) return -1;
	nowPrev = now;
    init = 1;

	*mst = (uint16_t)analogRead(PROBE_PIN);
    return 0;
}


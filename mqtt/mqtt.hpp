/*
 * mqtt.hpp
 *
 *  Created on: Jun 5, 2017
 *      Author: ipaev
 */

#ifndef MQTT_MQTT_HPP_
#define MQTT_MQTT_HPP_

#include <PubSubClient.h>

void setup_mqtt(uint32_t chip_id);
int loop_mqtt();
void publish_temperature_mqtt(float temp);
void publish_moisture_mqtt(uint16_t mst);
void mqtt_deinit();

#endif /* MQTT_MQTT_HPP_ */

/*
 * mqtt.hpp
 *
 *  Created on: Jun 5, 2017
 *      Author: ipaev
 */

#ifndef MQTT_MQTT_HPP_
#define MQTT_MQTT_HPP_

#include <PubSubClient.h>

PubSubClient * setup_mqtt();
int loop_mqtt();

#endif /* MQTT_MQTT_HPP_ */

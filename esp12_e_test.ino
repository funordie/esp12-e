#include "ds18b20.hpp"
#include "mqtt.hpp"

PubSubClient * client;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\n Starting...");

  client = setup_mqtt();

  setup_temperature();
  Serial.println("exit setup");
}

void loop() {

  // put your main code here, to run repeatedly:
	 loop_mqtt();

	 float temperature;
	 if(!loop_temperature(&temperature)) {
		 //valid temperature
		 client->publish("temperature", String(temperature).c_str(), true);
	 }
}

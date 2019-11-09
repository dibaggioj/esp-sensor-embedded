/*
 * data.h
 *
 *  Created on: Nov 9, 2019
 *      Author: johndibaggio
 */

#ifndef MAIN_MODELS_DATA_H_
#define MAIN_MODELS_DATA_H_

#include <string>
#include <esp_wifi_types.h>
#include <JSON.h>

struct temperature_reading {
	std::string device;
	float value;
	char unit;
	std::string location;
};

struct humidity_reading {
	std::string device;
	float value;
	std::string location;
};

struct data_point {
  std::string device;
  uint32_t timestamp;
  temperature_reading temperature;
  humidity_reading humidity;
};

class DataPoint {
public:
	//}
	/**
	 * 	{
		"device": "ESP32",
		"timestamp": {{timestamp}},
		"temperature": {
			"device": "DHT22",
			"value": 69.1,
			"unit": "F",
			"location": "home"
		},
		"humidity": {
			"device": "DHT22",
			"value": 0.86,
			"location": "home"
		}
	 */
	static JsonObject build_json(data_point d);
};



#endif /* MAIN_MODELS_DATA_H_ */

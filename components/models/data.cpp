/*
 * data.cpp
 *
 *  Created on: Nov 9, 2019
 *      Author: johndibaggio
 */

#include "data.hpp"

JsonObject DataPoint::build_json(data_point d) {
	JsonObject t_json = JSON::createObject();
	t_json.setString("device", d.temperature.device);
	t_json.setDouble("value", d.temperature.value);
	t_json.setString("unit", std::string(1, d.temperature.unit));
	t_json.setString("location", d.temperature.location);

	JsonObject h_json = JSON::createObject();
	h_json.setString("device", d.humidity.device);
	h_json.setDouble("value", d.humidity.value);
	h_json.setString("location", d.humidity.location);

	JsonObject d_json = JSON::createObject();
	d_json.setString("device", d.device);
	d_json.setLong("timestamp", d.timestamp);
	d_json.setObject("temperature", t_json);
	d_json.setObject("humidity", h_json);

	return d_json;
}



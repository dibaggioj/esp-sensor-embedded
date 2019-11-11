/*
 * rest_client.hpp
 *
 *  Created on: Nov 10, 2019
 *      Author: johndibaggio
 */

#ifndef COMPONENTS_CLIENTS_REST_CLIENT_HPP_
#define COMPONENTS_CLIENTS_REST_CLIENT_HPP_

#define REST_CLIENT_TAG "temperature_humidity_monitor http_rest_client"

#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_http_client.h>
#include <string>
#include <cstring>
#include <JSON.h>

#include "../models/data.hpp"
#include "sdkconfig.h"

class RestClient {
private:
	esp_http_client_config_t config;
	esp_http_client_handle_t client;
	esp_err_t _http_event_handler(esp_http_client_event_t *evt);
public:
	RestClient(const char *url, const char *host, int port, const char *username, const char *password, esp_http_client_auth_type_t auth_type, const char *base_api_path);
	esp_err_t post_json(const char *url, JsonObject json);
	esp_http_client_config_t get_config();
};



#endif /* COMPONENTS_CLIENTS_REST_CLIENT_HPP_ */

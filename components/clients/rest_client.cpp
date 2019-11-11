/*
 * rest_client.cpp
 *
 *  Created on: Nov 10, 2019
 *      Author: johndibaggio
 */

#include "rest_client.hpp"

esp_err_t RestClient::_http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(REST_CLIENT_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(REST_CLIENT_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(REST_CLIENT_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(REST_CLIENT_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(REST_CLIENT_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                // printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(REST_CLIENT_TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(REST_CLIENT_TAG, "HTTP_EVENT_DISCONNECTED");
//            int mbedtls_err = 0;
//            extern "C" {
//            		esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
//            }
//            if (err != 0) {
//                ESP_LOGI(APP_TAG, "Last esp error code: 0x%x", err);
//                ESP_LOGI(APP_TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
//            }
            break;
    }
    return ESP_OK;
}

RestClient::RestClient(const char *url, const char *host, int port, const char *username, const char *password, esp_http_client_auth_type_t auth_type, const char *base_api_path) {
	//	typedef struct {
	//	    const char                  *url;                /*!< HTTP URL, the information on the URL is most important, it overrides the other fields below, if any */
	//	    const char                  *host;               /*!< Domain or IP as string */
	//	    int                         port;                /*!< Port to connect, default depend on esp_http_client_transport_t (80 or 443) */
	//	    const char                  *username;           /*!< Using for Http authentication */
	//	    const char                  *password;           /*!< Using for Http authentication */
	//	    esp_http_client_auth_type_t auth_type;           /*!< Http authentication type, see `esp_http_client_auth_type_t` */
	//	    const char                  *path;               /*!< HTTP Path, if not set, default is `/` */
	//	    const char                  *query;              /*!< HTTP query */
	//	    const char                  *cert_pem;           /*!< SSL Certification, PEM format as string, if the client requires to verify server */
	//	    esp_http_client_method_t    method;                   /*!< HTTP Method */
	//	    int                         timeout_ms;               /*!< Network timeout in milliseconds */
	//	    bool                        disable_auto_redirect;    /*!< Disable HTTP automatic redirects */
	//	    int                         max_redirection_count;    /*!< Max redirection number, using default value if zero*/
	//	    http_event_handle_cb        event_handler;             /*!< HTTP Event Handle */
	//	    esp_http_client_transport_t transport_type;           /*!< HTTP transport type, see `esp_http_client_transport_t` */
	//	    int                         buffer_size;              /*!< HTTP buffer size (both send and receive) */
	//	    void                        *user_data;               /*!< HTTP user_data context */
	//	} esp_http_client_config_t;
	config = {
		"http://192.168.1.162:8080",
		_http_event_handler
	};
	client = esp_http_client_init(&config);

}

esp_http_client_config_t RestClient::get_config() {
	return config;
}

esp_err_t RestClient::post_json(const char *url, JsonObject json) {
	// TODO: convert json body to c-style string
	const char *post_data = "{\"channel\":\"A\",\"value\":\"X\"}";



	esp_http_client_set_url(client, "http://192.168.1.162:8080/api/rest/sensor-service/data");
	esp_http_client_set_method(client, HTTP_METHOD_POST);
	esp_http_client_set_post_field(client, post_data, std::strlen(post_data));
	esp_http_client_set_header(client, "Content-Type", "application/json");
	esp_err_t err = esp_http_client_perform(client);

	return err;
}



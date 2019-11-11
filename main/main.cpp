#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_http_client.h>
//#include <esp_tls.h>
#include <driver/gpio.h>
#include <nvs_flash.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <DHT.hpp>
#include <string>
#include <cstring>
//#include <stdlib.h>
#include <JSON.h>

#include "models/data.hpp"
#include "clients/rest_client.hpp"
#include "projectconfig.h"
#include "sdkconfig.h"

// TODO: update WiFi wrapper in CPP Utils to work with ESP-IDF v4+

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event:
 * are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

//static int s_retry_num = 0;

extern "C" {
	void app_main(void);
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    		ESP_LOGI(APP_TAG, "Disconnected from WiFi access point\n\n");
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
    		ESP_LOGI(APP_TAG, "Successfully connected to WiFi access point\n\n");
    		break;
    default:
        break;
    }
    return ESP_OK;
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                // printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(APP_TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(APP_TAG, "HTTP_EVENT_DISCONNECTED");
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

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    // CONFIG_MAX_STA_CONN
    int8_t rssi = 0;
    wifi_scan_threshold_t wifi_scan_threshold = {
    		rssi,
		WIFI_AUTH_WPA2_PSK
    };
    wifi_sta_config_t wifi_sta_config = {
		CONFIG_ESP_WIFI_SSID,
		CONFIG_ESP_WIFI_PASSWORD,
		WIFI_ALL_CHANNEL_SCAN,
		false,	// don't set MAC address of target access point
		0,		// default MAC address of target access point
		0,		// default access point channel
		0,		// unspecified listen interval for ESP32 station to receive beacon when WIFI_PS_MAX_MODEM is set (3 is the default when 0 is used)
		WIFI_CONNECT_AP_BY_SIGNAL,	// sort the connect AP in the list by rssi
		NULL		// TODO: determine why code isn't compiling when wifi_scan_threshold is used here
	};
    wifi_config_t wifi_config;
    wifi_config.sta = wifi_sta_config;

    ESP_LOGI(APP_TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void DHT_task(void *pvParameter)
{
    DHT dht;
    dht.setDHTgpio(GPIO_NUM_4);
    ESP_LOGI(APP_TAG, "Starting DHT Task\n\n");

    while (1)
    {
        ESP_LOGI(APP_TAG, "=== Reading DHT ===\n");
        int ret = dht.readDHT();

        dht.errorHandler(ret);
        float temp_f = dht.getTemperature() * 1.8 + 32;
        ESP_LOGI(APP_TAG, "Hum: %.1f Tmp: %.1f\n", dht.getHumidity(), temp_f);

        temperature_reading temp = {
        		"DHT22",
			temp_f,
			'F',
			"living room"
        };
        humidity_reading hum = {
        		"DHT22",
			dht.getHumidity(),
        		"living room"
        };
        data_point point = {
        		"ESP32 Thing",
			esp_log_timestamp(),
			temp,
			hum
        };

//        JsonObject json = DataPoint::build_json(point);

        esp_http_client_config_t config = {
			"http://192.168.1.162:8080",
			_http_event_handler
		};
		esp_http_client_handle_t client = esp_http_client_init(&config);

//TODO: make POST request
        const char *post_data = "{\"channel\":\"A\",\"value\":\"X\"}";

        esp_http_client_set_url(client, "http://192.168.1.162:8080/api/rest/sensor-service/data");
		esp_http_client_set_method(client, HTTP_METHOD_POST);
		esp_http_client_set_post_field(client, post_data, std::strlen(post_data));
		esp_http_client_set_header(client, "Content-Type", "application/json");
		esp_err_t err = esp_http_client_perform(client);

		if (err != 0) {
			ESP_LOGE(APP_TAG, "Last esp error code: 0x%x", err);
		} else {
			ESP_LOGI(APP_TAG, "Successful POST");
		}
        // TOOD: see examples/protocols/esp_http_client/main/esp_http_client_example.c
        // see

//        ESP_LOGI(APP_TAG, "JSON: %s", json.toStringUnformatted().c_str());

//        ESP_LOGI(APP_TAG, "Creating socket...\n");
//        Socket socket;
//        char ip_addr[]{ "192.168.1.162" };
//        ESP_LOGI(APP_TAG, "Making connection...\n");
//        int connection = socket.connect(ip_addr, 8080);
//        ESP_LOGI(APP_TAG, "Connection: %d\n", connection);

//        socket.close();
//        socket.~Socket();

        // -- wait at least 3 sec before reading again ------------
        // The interval of whole process must be beyond 2 seconds !!
        vTaskDelay(3000 / portTICK_RATE_MS);
    }
}


void app_main(void)
{
	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	initialise_wifi();

	esp_log_level_set(DHT::TAG, ESP_LOG_INFO);

	xTaskCreate(&DHT_task, "DHT_task", 2048, NULL, 5, NULL);
}

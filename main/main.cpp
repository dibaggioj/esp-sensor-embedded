#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <driver/gpio.h>
#include <nvs_flash.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <DHT.hpp>
#include <string>

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
    		ESP_LOGI(TAG, "Disconnected from WiFi access point\n\n");
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
    		ESP_LOGI(TAG, "Successfully connected to WiFi access point\n\n");
    		break;
    default:
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

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void DHT_task(void *pvParameter)
{
    DHT dht;
    dht.setDHTgpio(GPIO_NUM_4);
    ESP_LOGI(TAG, "Starting DHT Task\n\n");

    while (1)
    {
        ESP_LOGI(TAG, "=== Reading DHT ===\n");
        int ret = dht.readDHT();

        dht.errorHandler(ret);
        float temp_f = dht.getTemperature() * 1.8 + 32;
        ESP_LOGI(TAG, "Hum: %.1f Tmp: %.1f\n", dht.getHumidity(), temp_f);

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

	esp_log_level_set("*", ESP_LOG_INFO);

	xTaskCreate(&DHT_task, "DHT_task", 2048, NULL, 5, NULL);
}

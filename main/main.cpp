#include <esp_log.h>
#include <esp_err.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <DHT.hpp>
#include <string>
#include "sdkconfig.h"

static char TAG[]="temperature_humidity_monitor";

extern "C" {
	void app_main(void);
}

class Greeting {
public:
	void helloEnglish() {
		ESP_LOGD(TAG, "Hello %s", name.c_str());
	}

	void helloFrench() {
		ESP_LOGD(TAG, "Bonjour %s", name.c_str());
	}

	void setName(std::string name) {
		this->name = name;
	}
private:
	std::string name = "";

};


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
	Greeting myGreeting;
	myGreeting.setName("John");
	myGreeting.helloEnglish();
	myGreeting.helloFrench();

	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	esp_log_level_set("*", ESP_LOG_INFO);

	xTaskCreate(&DHT_task, "DHT_task", 2048, NULL, 5, NULL);
}


#include <esp_log.h>
#include <string>
#include "sdkconfig.h"


static char tag[]="temperature_humidity_monitor";

extern "C" {
	void app_main(void);
}

class Greeting {
public:
	void helloEnglish() {
		ESP_LOGD(tag, "Hello %s", name.c_str());
	}

	void helloFrench() {
		ESP_LOGD(tag, "Bonjour %s", name.c_str());
	}

	void setName(std::string name) {
		this->name = name;
	}
private:
	std::string name = "";

};

void app_main(void)
{
	Greeting myGreeting;
	myGreeting.setName("John");
	myGreeting.helloEnglish();
	myGreeting.helloFrench();
}


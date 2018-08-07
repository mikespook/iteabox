#ifndef ITEA_WIFI
#define ITEA_WIFI

#include <SPI.h>
#include <ESP8266WiFi.h>

#include "iTeaConfig.h"

class iTeaWiFiClass {
	public:
		iTeaWiFiClass();
		void init(iTeaConfigClass *);

		uint8_t connect();
		uint8_t wait(int);
	private:
		iTeaConfigClass *_config;
		uint8_t _macAddr[6];
};

static iTeaWiFiClass iTeaWiFi;
#endif // ITEA_WIFI

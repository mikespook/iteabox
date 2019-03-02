#ifndef ITEA_SETUP
#define ITEA_SETUP

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "iTeaConfig.h"
#include "iTeaHandler.h"
#include "iTeaHTML.h"

// #define ITEA_SETUP_BTN 15 // D10
#define ITEA_SETUP_BTN 5 // R1:D3 R2:D1
#define ITEA_STATE_SETUP 5


String redirectPage(String, String, String); 

// setup service class
class iTeaSetupClass {
	public:
		iTeaSetupClass();
		void init(iTeaConfigClass *);

		uint8_t setup();
		uint8_t loop();
	private:
		void _handleRoot();
		void _handlePost();
		void _handleRestart();
		void _handleNotFound();
		void _handleReset();
		void _handleDebug();

		ESP8266WebServer _server;
		iTeaConfigClass *_config;
		uint8_t _state = ITEA_STATE_SETUP;
};

static iTeaSetupClass iTeaSetup;
#endif // ITEA_SETUP

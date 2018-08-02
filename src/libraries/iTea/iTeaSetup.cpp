#include "iTeaSetup.h"

// Public
iTeaSetupClass::iTeaSetupClass() {
	pinMode(ITEA_SETUP_BTN, INPUT);
}

void iTeaSetupClass::init(iTeaConfigClass *config) {
	_config = config;
}

uint8_t iTeaSetupClass::setup() {
	// read the eeprom
	char ssid[ITEA_WIFI_SSID_SIZE] = {0};
	_config->getSSID(ssid);
	if (strlen(ssid) == 0) {
		Serial.println("Empty SSID");
		goto SETUP;
	}
	for(int i = 0; i < 10; i ++) {
		if(digitalRead(ITEA_SETUP_BTN) == HIGH) {
			Serial.println("Setup button pressed");
			goto SETUP;
		}
		delay(200);
	}	
	return ITEA_STATE_RUN;
SETUP:
	// if the configration is empty or setup button was pressed down
	//start WiFi AP
	WiFi.mode(WIFI_AP);
	WiFi.disconnect();
	WiFi.softAP(ITEA_WIFI_SSID, ITEA_WIFI_PASS);
	IPAddress apIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.print(apIP);
	//start HTTP service
	std::function<void(void)> hRoot = [&]{this->_handleRoot();};
	_server.on("/", hRoot);
	std::function<void(void)> hPost = [&]{this->_handlePost();};
	_server.on("/post", HTTP_POST, hPost);
	std::function<void(void)> hRestart = [&]{this->_handleRestart();};
	_server.on("/restart", hRestart);
	std::function<void(void)> hReset = [&]{this->_handleReset();};
	_server.on("/reset", hReset);
	std::function<void(void)> hDebug = [&]{this->_handleDebug();};
	_server.on("/debug", hDebug);
	std::function<void(void)> hNotFound = [&]{this->_handleNotFound();};
	_server.onNotFound(hNotFound);
	_server.begin();
	return ITEA_STATE_SETUP;
}

uint8_t iTeaSetupClass::loop() {
	_server.handleClient();
	return _state;
}

void iTeaSetupClass::_handleRoot() {
	Serial.println("Root handle");
	String page = String(ITEA_SETUP_PAGE_ROOT);
	char field[ITEA_EEPROM_SIZE] = {0};
	_config->getSSID(field);
	page.replace("$SSID$", String(field));
	_config->getPass(field);
	page.replace("$PASS$", String(field));

	_config->getMQTTAddr(field);
	page.replace("$MQTTADDR$", String(field));
	_config->getMQTTPort(field);
	page.replace("$MQTTPORT$", String(field));
	_config->getMQTTUser(field);
	page.replace("$MQTTUSER$", String(field));
	_config->getMQTTPass(field);
	page.replace("$MQTTPASS$", String(field));
	_config->getMQTTTopic(field);
	page.replace("$MQTTTOPIC$", String(field));
	_config->getMQTTTopicHeartbeat(field);
	page.replace("$MQTTTOPICHEARTBEAT$", String(field));
	_config->getMQTTHeartbeatTick(field);
	page.replace("$MQTTHEARTBEATTICK$", String(field));
	_config->getMQTTClientId(field);
	page.replace("$MQTTCLIENTID$", String(field));
	_server.send (200, "text/html", page);
}

void iTeaSetupClass::_handlePost() {
	Serial.println("Set handle");
	_server.sendHeader("Location", "/");
	_server.send (302, "text/plain", "Config updated...\n\n");

	_config->setSSID(_server.arg("ssid").c_str());
	_config->setPass(_server.arg("pass").c_str());
	_config->setMQTTAddr(_server.arg("mqtt-addr").c_str());
	_config->setMQTTPort(_server.arg("mqtt-port").c_str());
	_config->setMQTTUser(_server.arg("mqtt-user").c_str());
	_config->setMQTTPass(_server.arg("mqtt-pass").c_str());
	_config->setMQTTTopic(_server.arg("mqtt-topic").c_str());
	_config->setMQTTTopicHeartbeat(_server.arg("mqtt-topic-heartbeat").c_str());
	_config->setMQTTHeartbeatTick(_server.arg("mqtt-heartbeat-tick").c_str());
	_config->setMQTTClientId(_server.arg("mqtt-client-id").c_str());
	_config->dump();
}

void iTeaSetupClass::_handleReset() {
	Serial.println("Reset handle");
	_server.send (200, "text/html",
			redirectPage("3", "/", "Resetting..."));
	_config->clean();
	_state = ITEA_STATE_INIT;
}

void iTeaSetupClass::_handleRestart() {
	Serial.println("Restart handle");
	_server.send (200, "text/html",
			redirectPage("30", "javascript:window.close();", "Restarting..."));
	ESP.restart();
	_state = ITEA_STATE_INIT;
}

void iTeaSetupClass::_handleDebug() {
	Serial.println("Debug handle");
	_server.send (200, "text/plain", String(_config->debug()));
}

void iTeaSetupClass::_handleNotFound() {
	Serial.println("Not Found handle");
	_server.send ( 404, "text/html",
			redirectPage("5", "/", "File Not Found"));
}

String redirectPage(String delay, String url, String text) {
	String page = String(ITEA_SETUP_PAGE_REDIRECT);
	page.replace("$DELAY$", delay);
	page.replace("$URL$", url);
	page.replace("$TEXT$", text);
	return page;
}

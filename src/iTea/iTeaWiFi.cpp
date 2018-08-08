#include "iTeaWiFi.h"

iTeaWiFiClass::iTeaWiFiClass() {

}

void iTeaWiFiClass::init(iTeaConfigClass *config) {
	_config = config;
}
// Connect WiFi
uint8_t iTeaWiFiClass::connect() {
	if (WiFi.isConnected()) {
		return WL_CONNECTED;
	}
	// read SSID and Password from the config
	// Connect WiFi
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	
	char ssid[ITEA_WIFI_SSID_SIZE] = {0};
	_config->getSSID(ssid);
	char pass[ITEA_WIFI_PASS_SIZE] = {0};
	_config->getPass(pass);
	Serial.printf("[WiFi] Connecting: SSID=%s ", ssid);	
	WiFi.begin(ssid, pass);
	return wait(300);
}

uint8_t iTeaWiFiClass::wait(int tick) {
	String macStr = WiFi.macAddress();
	memcpy(_macAddr, macStr.c_str(), 6);
	// Wait max to tick/10 seconds
	for (int i = 0; i < tick; i ++) {
		Serial.printf(".");
		if (WiFi.isConnected()) {
			Serial.printf("\n[WiFi] Connected: MAC=%s; SSID=%s; IP=", macStr.c_str(), WiFi.SSID().c_str());
			Serial.println(WiFi.localIP());
			return WL_CONNECTED;
		}
		delay(100);
	}
	Serial.println();
	int status = WiFi.status();
	switch (status) {
		case WL_NO_SSID_AVAIL:
			Serial.printf("[WiFi] No SSID: MAC=%s; SSID=%s\n", macStr.c_str(), WiFi.SSID().c_str());
			break;
		case WL_CONNECTION_LOST:
			Serial.printf("[WiFi] Connection Lost: MAC=%s; SSID=%s\n", macStr.c_str(), WiFi.SSID().c_str());
			break;
		case WL_CONNECT_FAILED:
			Serial.printf("[WiFi] Connect Failed: MAC=%s; SSID=%s\n", macStr.c_str(), WiFi.SSID().c_str());
			break;
		default:
			Serial.printf("[WiFi] Connect Issue: MAC=%s; SSID=%s; Status=%d\n", macStr.c_str(), WiFi.SSID().c_str(), status);
			break;
	}
	return status;
}

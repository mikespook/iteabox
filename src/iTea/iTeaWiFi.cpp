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

	// Connect WiFi
	WiFi.disconnect();
	WiFi.mode(WIFI_STA);

	// read SSID and Password from the config
	char ssid[ITEA_WIFI_SSID_SIZE] = {0};
	_config->getSSID(ssid);
	char pass[ITEA_WIFI_PASS_SIZE] = {0};
	_config->getPass(pass);
	Serial.printf("[WiFi] Connecting: SSID=%s ", ssid);	
	WiFi.begin(ssid, pass);
	return wait(300);
}

// Connect WiFi
uint8_t iTeaWiFiClass::beginWPS() {
	if (WiFi.isConnected()) {
		return WL_CONNECTED;
	}
	// Connect WiFi
	WiFi.disconnect();
	WiFi.mode(WIFI_STA);
	Serial.print("[WiFi] WPS Connecting...");
	if (WiFi.beginWPSConfig()) {
		Serial.print("SSID=");
		Serial.println(WiFi.SSID());	
		return WL_CONNECTED;
	}
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

void iTeaWiFiClass::setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

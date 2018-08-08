#include "iTeaConfig.h"

#define LOAD(v, offset, size)\
	for(int x = 0; x < size; x++) { \
		v[x] = char(EEPROM.read(x + offset));}\
	v[offset + size] = '\0';

#define DUMP(v, offset, size)\
	for(int x = 0; x < size; x++) { EEPROM.write(x + offset, v[x]);}

#define SET(name, v, size) \
	void iTeaConfigClass::set##name(const char *v) {strncpy(_##v, v, size);}

#define GET(name, v) \
	void iTeaConfigClass::get##name(char *v) {strcpy(v, _##v);}

// Public
iTeaConfigClass::iTeaConfigClass() {
	randomSeed(analogRead(ITEA_RAND_PIN));
	EEPROM.begin(ITEA_EEPROM_SIZE);
}

void iTeaConfigClass::load() {
	LOAD(_wifiSSID, ITEA_WIFI_SSID_OFFSET, ITEA_WIFI_SSID_SIZE);
	LOAD(_wifiPass, ITEA_WIFI_PASS_OFFSET, ITEA_WIFI_PASS_SIZE);

	LOAD(_mqttAddr, ITEA_MQTT_ADDR_OFFSET, ITEA_MQTT_ADDR_SIZE);
	LOAD(_mqttPort, ITEA_MQTT_PORT_OFFSET, ITEA_MQTT_PORT_SIZE);
	LOAD(_mqttUser, ITEA_MQTT_USER_OFFSET, ITEA_MQTT_USER_SIZE);
	LOAD(_mqttPass, ITEA_MQTT_PASS_OFFSET, ITEA_MQTT_PASS_SIZE);
	LOAD(_mqttTopicHeartbeat, ITEA_MQTT_TOPIC_HEARTBEAT_OFFSET, ITEA_MQTT_TOPIC_SIZE);
	LOAD(_mqttHeartbeatTick, ITEA_MQTT_HBTICK_OFFSET, ITEA_MQTT_HBTICK_SIZE);
	LOAD(_mqttClientId, ITEA_MQTT_CLIENTID_OFFSET, ITEA_MQTT_CLIENTID_SIZE);

	if (strlen(_mqttTopicHeartbeat) == 0) {
		sprintf(_mqttTopicHeartbeat, ITEA_MQTT_TOPIC_HEARTBEAT_DEFAULT);
	}
	if (strlen(_mqttHeartbeatTick) == 0) {
		sprintf(_mqttHeartbeatTick, ITEA_MQTT_HBTICK_DEFAULT);
	}

	if (strlen(_mqttClientId) == 0) {
		gen_random(_mqttClientId, ITEA_MQTT_CLIENTID_SIZE);
	}
}

void iTeaConfigClass::dump() {
	clean();
	DUMP(_wifiSSID, ITEA_WIFI_SSID_OFFSET, ITEA_WIFI_SSID_SIZE);
	DUMP(_wifiPass, ITEA_WIFI_PASS_OFFSET, ITEA_WIFI_PASS_SIZE);

	DUMP(_mqttAddr, ITEA_MQTT_ADDR_OFFSET, ITEA_MQTT_ADDR_SIZE);
	DUMP(_mqttPort, ITEA_MQTT_PORT_OFFSET, ITEA_MQTT_PORT_SIZE);
	DUMP(_mqttUser, ITEA_MQTT_USER_OFFSET, ITEA_MQTT_USER_SIZE);
	DUMP(_mqttPass, ITEA_MQTT_PASS_OFFSET, ITEA_MQTT_PASS_SIZE);
	DUMP(_mqttTopicHeartbeat, ITEA_MQTT_TOPIC_HEARTBEAT_OFFSET, ITEA_MQTT_TOPIC_SIZE);
	DUMP(_mqttHeartbeatTick, ITEA_MQTT_HBTICK_OFFSET, ITEA_MQTT_HBTICK_SIZE);
	DUMP(_mqttClientId, ITEA_MQTT_CLIENTID_OFFSET, ITEA_MQTT_CLIENTID_SIZE);
	EEPROM.commit();
}

SET(SSID, wifiSSID, ITEA_WIFI_SSID_SIZE);
SET(Pass, wifiPass, ITEA_WIFI_PASS_SIZE);
SET(MQTTAddr, mqttAddr, ITEA_MQTT_ADDR_SIZE);
SET(MQTTPort, mqttPort, ITEA_MQTT_PORT_SIZE);
SET(MQTTUser, mqttUser, ITEA_MQTT_USER_SIZE);
SET(MQTTPass, mqttPass, ITEA_MQTT_PASS_SIZE);
SET(MQTTTopicHeartbeat, mqttTopicHeartbeat, ITEA_MQTT_TOPIC_SIZE);
SET(MQTTHeartbeatTick, mqttHeartbeatTick, ITEA_MQTT_HBTICK_SIZE);
SET(MQTTClientId, mqttClientId, ITEA_MQTT_CLIENTID_SIZE);

GET(SSID, wifiSSID);
GET(Pass, wifiPass);
GET(MQTTAddr, mqttAddr);
GET(MQTTPort, mqttPort);
GET(MQTTUser, mqttUser);
GET(MQTTPass, mqttPass);
GET(MQTTTopicHeartbeat, mqttTopicHeartbeat);
GET(MQTTHeartbeatTick, mqttHeartbeatTick);
GET(MQTTClientId, mqttClientId);

void iTeaConfigClass::clean() {
	for (int i = 0; i < ITEA_EEPROM_SIZE; i++) {
		EEPROM.write(i, 0);
	}
	EEPROM.commit();
}

String iTeaConfigClass::debug() {
	String eeprom;
	for (int i = 0; i < ITEA_EEPROM_SIZE; i++) {
		uint8_t c = EEPROM.read(i);
		if (c >= 0x20 && c <= 0x7E) {
			eeprom += String((char)c);
		} else {
			eeprom += String(c, HEX);
		}
		eeprom += "\t";
		if (i % 16 == 15) {
			eeprom += "\n";
		}
	}
	return eeprom;
}

void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[random(0, sizeof(alphanum) - 1)];
    }
    s[len] = 0;
}

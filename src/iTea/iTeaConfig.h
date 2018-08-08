#ifndef ITEA_CONFIG
#define ITEA_CONFIG

#define ITEA_WIFI_SSID "ITEA-SetupService"
#define ITEA_WIFI_PASS ""

// WiFi
#define ITEA_WIFI_SSID_OFFSET	0
#define ITEA_WIFI_SSID_SIZE	32
#define ITEA_WIFI_PASS_OFFSET ITEA_WIFI_SSID_OFFSET + ITEA_WIFI_SSID_SIZE
#define ITEA_WIFI_PASS_SIZE	64

// MQTT
// addr
#define ITEA_MQTT_ADDR_OFFSET ITEA_WIFI_PASS_OFFSET + ITEA_WIFI_PASS_SIZE
#define ITEA_MQTT_ADDR_SIZE 64
// port
#define ITEA_MQTT_PORT_OFFSET ITEA_MQTT_ADDR_OFFSET + ITEA_MQTT_ADDR_SIZE
#define ITEA_MQTT_PORT_SIZE 5
// user
#define ITEA_MQTT_USER_OFFSET ITEA_MQTT_PORT_OFFSET + ITEA_MQTT_PORT_SIZE
#define ITEA_MQTT_USER_SIZE 32
// pass
#define ITEA_MQTT_PASS_OFFSET ITEA_MQTT_USER_OFFSET + ITEA_MQTT_USER_SIZE
#define ITEA_MQTT_PASS_SIZE 32
// heartbeat
#define ITEA_MQTT_TOPIC_SIZE 32
#define ITEA_MQTT_TOPIC_HEARTBEAT_DEFAULT "itea:heartbeat:pub"
#define ITEA_MQTT_TOPIC_HEARTBEAT_OFFSET ITEA_MQTT_PASS_OFFSET + ITEA_MQTT_PASS_SIZE
#define ITEA_MQTT_HBTICK_OFFSET ITEA_MQTT_TOPIC_HEARTBEAT_OFFSET + ITEA_MQTT_TOPIC_SIZE
#define ITEA_MQTT_HBTICK_SIZE 5
#define ITEA_MQTT_HBTICK_DEFAULT "60"
// client id
#define ITEA_MQTT_CLIENTID_OFFSET ITEA_MQTT_HBTICK_OFFSET + ITEA_MQTT_HBTICK_SIZE
#define ITEA_MQTT_CLIENTID_SIZE 32
// random pin
#define ITEA_RAND_PIN 0

// rom size
#define ITEA_EEPROM_SIZE ITEA_MQTT_CLIENTID_OFFSET + ITEA_MQTT_CLIENTID_SIZE

#include <SPI.h>
#include <EEPROM.h>

// Config class
class iTeaConfigClass {
	public:
		iTeaConfigClass();
		
		void clean();
		void load();
		void dump();
		String debug();

		void setSSID(const char*);
		void setPass(const char*);
	
		void setMQTTAddr(const char*);
		void setMQTTPort(const char*);
		void setMQTTUser(const char*);
		void setMQTTPass(const char*);
		void setMQTTTopicHeartbeat(const char*);
		void setMQTTHeartbeatTick(const char*);
		void setMQTTClientId(const char*);

		void getSSID(char *);
		void getPass(char *);
		
		void getMQTTAddr(char *);
		void getMQTTPort(char *);
		void getMQTTUser(char *);
		void getMQTTPass(char *);
		void getMQTTTopicHeartbeat(char *);
		void getMQTTHeartbeatTick(char *);
		void getMQTTClientId(char*);

	private:
		char _wifiSSID[ITEA_WIFI_SSID_SIZE + 1] = {0};
		char _wifiPass[ITEA_WIFI_PASS_SIZE + 1] = {0};

		char _mqttAddr[ITEA_MQTT_ADDR_SIZE + 1] = {0};
		char _mqttPort[ITEA_MQTT_PASS_SIZE + 1] = {0};	
		char _mqttUser[ITEA_MQTT_USER_SIZE + 1] = {0};
		char _mqttPass[ITEA_MQTT_PASS_SIZE + 1] = {0};
		char _mqttTopicHeartbeat[ITEA_MQTT_TOPIC_SIZE + 1] = {0};	
		char _mqttHeartbeatTick[ITEA_MQTT_HBTICK_SIZE + 1] = {0};	
		char _mqttClientId[ITEA_MQTT_CLIENTID_SIZE + 1] = {0};
};
void printHex(char *buffer, int bufferSize);
String sprintHex(char *buffer, int bufferSize);
void gen_random(char *s, const int len);
static iTeaConfigClass iTeaConfig;
#endif // ITEA_CONFIG

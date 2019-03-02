#ifndef ITEA_MQTT
#define ITEA_MQTT

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "iTeaConfig.h"
#include "iTeaHandler.h"

#define ITEA_STATE_MQTT_CONNECT 10
#define ITEA_STATE_MQTT_DISCONNECT 11

// callback function 
typedef void (*CallbackHandler)(const char*, const uint8_t*, unsigned int);

// iTeaMQTT class
class iTeaMQTTClass {
	public:
		iTeaMQTTClass();
		void init(iTeaConfigClass *config);
		void subscribe(const char*subtopic, CallbackHandler callback);
		
		uint8_t setup();
		uint8_t loop();
		int state();

		bool publish(const char* topic, const char* payload);
	
	private:
		iTeaConfigClass *_config;
		WiFiClientSecure _client;
		PubSubClient _mqtt = PubSubClient(_client);

		char _clientId[ITEA_MQTT_CLIENTID_SIZE + 1] = {0};
		char _user[ITEA_MQTT_USER_SIZE + 1] = {0};
		char _pass[ITEA_MQTT_PASS_SIZE + 1] = {0};		

		char _pubTopicHeartbeat[ITEA_MQTT_TOPIC_SIZE + 1] = {0};
		unsigned long _heartbeatTick = 60;
		unsigned long _lastHeartbeat = 0;

		char _subTopic[ITEA_MQTT_TOPIC_SIZE + 1] = {0};

		CallbackHandler _callback = NULL;
		uint8_t _reconnect();
};

static iTeaMQTTClass iTeaMQTT;
void setClock();
#endif // ITEA_MQTT

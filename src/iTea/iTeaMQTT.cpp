#include "iTeaMQTT.h"
#include "ca.h"

// Public
iTeaMQTTClass::iTeaMQTTClass() {
}

void iTeaMQTTClass::init(iTeaConfigClass *config) {
	_config = config;
}

void iTeaMQTTClass::subscribe(const char *subtopic, CallbackHandler callback) {
	_callback = callback;
	strcpy(_subTopic, subtopic);
}

uint8_t iTeaMQTTClass::setup() {
	_client.setInsecure();
    X509List x509(rootCA, rootCA_len);
    _client.setTrustAnchors(&x509);
	char addr[ITEA_MQTT_ADDR_SIZE + 1] = {0};
	_config->getMQTTAddr(addr);
	char port[ITEA_MQTT_PORT_SIZE + 1] = {0};
	_config->getMQTTPort(port);	
	_mqtt.setServer(addr, atoi(port));
	Serial.printf("Server: %s(%d)\n", addr, atoi(port));
	
	// heartbeat
	_config->getMQTTTopicHeartbeat(_pubTopicHeartbeat);
	char tick[ITEA_MQTT_HBTICK_SIZE + 1] = {0};
	_config->getMQTTHeartbeatTick(tick);
	_heartbeatTick = atoi(tick);

	_config->getMQTTClientId(_clientId);
	Serial.printf("Client ID: %s\n", _clientId);
	_config->getMQTTUser(_user);
	_config->getMQTTPass(_pass);
	Serial.printf("User: %s %s\n", _user, _pass);
	
	_mqtt.setCallback([this](char* topic, uint8_t* payload, unsigned int length){
		if (_callback != NULL) {
			_callback(topic, payload, length);
		}
	});
	return _reconnect();
}

uint8_t iTeaMQTTClass::loop() {
	if (_reconnect() != ITEA_STATE_MQTT_CONNECT) {
		return ITEA_STATE_MQTT_DISCONNECT;
	}
	if (_mqtt.state() != MQTT_CONNECTED) {
		return ITEA_STATE_MQTT_DISCONNECT;
	}
	if (!_mqtt.loop()) {
		return ITEA_STATE_MQTT_DISCONNECT;
	}
	unsigned long now = millis();
	// customise frequency of heartbeat
	if ((now < _lastHeartbeat) || (now - _lastHeartbeat) > 1000 * _heartbeatTick) {
		// customise heartbeat topic
		_mqtt.publish(_pubTopicHeartbeat, _clientId);
		_lastHeartbeat = now;
	}
	return ITEA_STATE_RUN;
}

bool iTeaMQTTClass::publish(const char *topic, const char* payload) {
	return _mqtt.publish(topic, payload);
}

int iTeaMQTTClass::state() {
	return _mqtt.state();
}

uint8_t iTeaMQTTClass::_reconnect() {	
	if (_mqtt.connected()) {
		return ITEA_STATE_MQTT_CONNECT;
	}
	int r;
	if (strlen(_user) == 0) {
		r = _mqtt.connect(_clientId);
	} else {
		r = _mqtt.connect(_clientId, _user, _pass);
	}
	if (r) {
		_mqtt.subscribe(_subTopic);
		return ITEA_STATE_MQTT_CONNECT;
	}
	return ITEA_STATE_MQTT_DISCONNECT;
}

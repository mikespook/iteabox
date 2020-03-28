#include <iTeaHandler.h>
#include <iTeaConfig.h>
#include <iTeaSetup.h>
#include <iTeaWiFi.h>
#include <iTeaMQTT.h>

#define SERIAL_SPEED 115200

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.printf("Serial speed: %d\n", SERIAL_SPEED);    
  
  Serial.printf("Load Config ... ");
  iTeaConfig.load();  
  Serial.println("Done");
  Serial.printf("Init Handler ... ");
  iTeaHandler.setup();
  iTeaHandler.add(ITEA_STATE_INIT, initHandler);
  iTeaHandler.add(ITEA_STATE_SETUP, setupHandler);
  iTeaHandler.add(ITEA_STATE_RUN, runHandler);
  Serial.println("Done");
}

void loop() {
  iTeaHandler.loop(NULL);
}

uint8_t runHandler(uint8_t state, void *params ...) {
  if (WL_CONNECTED == iTeaWiFi.connect()) {
    iTeaMQTT.loop();
  }
  return ITEA_STATE_RUN; 
}

void callback(const char* topic, const uint8_t* payload, unsigned int) {
  Serial.printf("MQTT: %s\n", payload);
}

uint8_t initHandler(uint8_t state, void *params ...) { 
  Serial.println("Init..."); 
  iTeaSetup.init(&iTeaConfig);
  iTeaWiFi.init(&iTeaConfig);
  iTeaMQTT.init(&iTeaConfig);
  uint8_t r = iTeaSetup.setup();
  if (r != ITEA_STATE_SETUP) {
    Serial.println("Run...normal"); 
    #ifdef USE_WPS
    if (WL_CONNECTED != iTeaWiFi.connectWPS()) {  
    #else
    if (WL_CONNECTED != iTeaWiFi.connect()) {
    #endif
      Serial.printf("WiFi Error\n");
      return ITEA_STATE_RUN;
    }
    iTeaWiFi.setClock();
    iTeaMQTT.subscribe("itea:pump:sub", callback);
    r = iTeaMQTT.setup();
    if (r != ITEA_STATE_MQTT_CONNECT) {
      Serial.printf("MQTT Connection Error %d\n", iTeaMQTT.state());
    }
  }
  return ITEA_STATE_RUN;  
}

uint8_t setupHandler(uint8_t state, void *params ...) {  
  Serial.println("Setup..."); 
  return iTeaSetup.loop();
}

#define DEBUG_ESP_HTTP_CLIENT 1
#define DEBUG_ESP_PORT 1
#define ITEA_DEBUG 1
#define SERIAL_SPEED 115200

// pins
#define WATER_SENSOR_PIN 4 // GPIO4, D4
#define RELAY_PIN 16 // GPIO16, D2
// state
#define DP_PUMPON 50
#define DP_PUMPOFF 51
// config
#define DP_DELAY 30000
#define DP_INTERVAL 5000

#include <iTeaHandler.h>
#include <iTeaConfig.h>
#include <iTeaSetup.h>
#include <iTeaWiFi.h>

unsigned long pumpOnTime, pumpOffTime;
bool pump = false;

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.printf("Serial speed: %d\n", SERIAL_SPEED);    
  
  pinMode(WATER_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  Serial.printf("Load Config ... ");
  iTeaConfig.load();  
  Serial.println("Done");
  Serial.printf("Init Handler ... ");
  iTeaHandler.setup();
  iTeaHandler.add(ITEA_STATE_INIT, initHandler);
  iTeaHandler.add(ITEA_STATE_SETUP, setupHandler);
  iTeaHandler.add(ITEA_STATE_RUN, runHandler);
  iTeaHandler.add(DP_PUMPON, pumpOnHandler);
  iTeaHandler.add(DP_PUMPOFF, pumpOffHandler);
  Serial.println("Done");
}

void loop() {
  iTeaHandler.loop(NULL);
}

uint8_t pumpOnHandler(uint8_t state, void *params ...) {
  Serial.printf("Pump on at %d\n", pumpOnTime);
  digitalWrite(RELAY_PIN, HIGH);
  pump = true;
  return ITEA_STATE_RUN;
}

uint8_t pumpOffHandler(uint8_t state, void *params ...) {
  Serial.printf("Countdown: %d\n", millis() - pumpOffTime);
  if ((millis() - pumpOffTime) > DP_DELAY) {
    Serial.printf("Pump off at %d\n", pumpOffTime); 
    digitalWrite(RELAY_PIN, LOW);
  }
  return ITEA_STATE_RUN;
}

uint8_t runHandler(uint8_t state, void *params ...) {
  if (WL_CONNECTED == iTeaWiFi.connect()) {
    delay(DP_INTERVAL);
    if (HIGH == digitalRead(WATER_SENSOR_PIN)) {
      pumpOnTime = millis();
      return DP_PUMPON;
    } else {
      if (pump == true) {
        pump = false;
        pumpOffTime = millis();
        Serial.printf("Off singal at %d\n", pumpOffTime);
      }
      return DP_PUMPOFF;
    }
  }
  return ITEA_STATE_RUN; 
}

uint8_t initHandler(uint8_t state, void *params ...) {  
  iTeaSetup.init(&iTeaConfig);
  iTeaWiFi.init(&iTeaConfig);
  return iTeaSetup.setup();  
}

uint8_t setupHandler(uint8_t state, void *params ...) {  
  return iTeaSetup.loop();
}
#define SERIAL_SPEED 115200

// pins
#define PIN_PAW_SWITCH 4 // GPIO4, D4
#define RELAY_PIN 16 // GPIO16, D2
// state
#define PF_ON 50
// config
#define PF_DELAY 30000
#define PF_INTERVAL 5000

#include <iTeaHandler.h>
#include <iTeaConfig.h>
#include <iTeaSetup.h>
#include <iTeaWiFi.h>
#include <iTeaMQTT.h>

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
  iTeaHandler.add(PF_ON, feedHandler);  
  Serial.println("Done");
}

void loop() {
  iTeaHandler.loop(NULL);
}

uint8_t feedHandler(uint8_t state, void *params ...) {
  iTeaMQTT.publish("itea::pub", "+");
  Serial.printf("Pump on at %d\n", pumpOnTime);
  digitalWrite(RELAY_PIN, HIGH);
  pump = true;
  return ITEA_STATE_RUN;
}

uint8_t pumpOffHandler(uint8_t state, void *params ...) {
  Serial.printf("Countdown: %d\n", millis() - pumpOffTime);
  if ((millis() - pumpOffTime) > DP_DELAY) {
    iTeaMQTT.publish("itea:pump:pub", "-");
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
    iTeaMQTT.loop();
  }
  return ITEA_STATE_RUN; 
}

void callback(const char* topic, const uint8_t* payload, unsigned int) {
  iTeaHandler.call(DP_PUMPON, (void *)topic, (void *)payload);
}

uint8_t initHandler(uint8_t state, void *params ...) {  
  iTeaSetup.init(&iTeaConfig);
  iTeaWiFi.init(&iTeaConfig);
  iTeaMQTT.init(&iTeaConfig, callback);
  return iTeaSetup.setup();  
}

uint8_t setupHandler(uint8_t state, void *params ...) {  
  return iTeaSetup.loop();
}

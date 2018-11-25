#include <iTeaHandler.h>
#include <iTeaConfig.h>
#include <iTeaSetup.h>
#include <iTeaWiFi.h>
#include <iTeaMQTT.h>
#include <Stepper.h>

#define SERIAL_SPEED 115200

// pins
#define PIN_PAW_SWITCH D4 // GPIO4, D4
// state
#define PF_FEED 50
// config
#define PF_INTERVAL 100
#define PF_STEP_INTERVAL 5
#define PF_STEPPER_BEGIN 0
#define PF_STEPPER_END 2200
#define PF_STEPPER_STEP 1



const int stepsPerRevolution = 16;
Stepper myStepper(stepsPerRevolution, D2, D6, D7, D8);
int steps = PF_STEPPER_BEGIN;
bool mqttFeed = false;

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.printf("Serial speed: %d\n", SERIAL_SPEED);    
  
  pinMode(PIN_PAW_SWITCH, INPUT_PULLUP);

  myStepper.setSpeed(660);
  
  Serial.printf("Load Config ... ");
  iTeaConfig.load();  
  Serial.println("Done");
  Serial.printf("Init Handler ... ");
  iTeaHandler.setup();
  iTeaHandler.add(ITEA_STATE_INIT, initHandler);
  iTeaHandler.add(ITEA_STATE_SETUP, setupHandler);
  iTeaHandler.add(ITEA_STATE_RUN, runHandler);
  iTeaHandler.add(PF_FEED, feedHandler);  
  Serial.println("Done");
}

void loop() {
  iTeaHandler.loop(NULL);
}

uint8_t feedHandler(uint8_t state, void *params ...) {
  if (steps == PF_STEPPER_BEGIN) {
    iTeaMQTT.publish("itea:petsfeeder:pub", "+"); 
    Serial.printf("Feed"); 
  } 
  if (steps < PF_STEPPER_END ) {
    myStepper.step(PF_STEPPER_STEP);
    steps ++;
    return PF_FEED;
  }
  steps = PF_STEPPER_BEGIN;
  Serial.println("...Done");
  mqttFeed = false;
  return ITEA_STATE_RUN;
}

uint8_t runHandler(uint8_t state, void *params ...) {
  iTeaWiFi.connect();
  if (LOW == digitalRead(PIN_PAW_SWITCH)) {
    Serial.printf("Feed at %d\n", millis());
    return PF_FEED;
  }
  if (WL_CONNECTED == iTeaWiFi.connect()) {
    iTeaMQTT.loop();
  }
  if (mqttFeed) {
    return PF_FEED;  
  }
  return ITEA_STATE_RUN; 
}

void callback(const char* topic, const uint8_t* payload, unsigned int) {
  Serial.printf("MQTT Feed at %d\n", millis());
  mqttFeed = true;
}

uint8_t initHandler(uint8_t state, void *params ...) {  
  iTeaSetup.init(&iTeaConfig);
  iTeaWiFi.init(&iTeaConfig);
  iTeaMQTT.init(&iTeaConfig);
  iTeaMQTT.subscribe("itea:petsfeeder:sub", callback);
  uint8_t r = iTeaSetup.setup();
  if (r != ITEA_STATE_SETUP) {
    iTeaWiFi.connect();
    iTeaMQTT.setup();
  } 
  return r;
}

uint8_t setupHandler(uint8_t state, void *params ...) {  
  return iTeaSetup.loop();
}

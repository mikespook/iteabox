#include <iTeaHandler.h>
#include <iTeaConfig.h>
#include <iTeaSetup.h>
#include <iTeaWiFi.h>
#include <iTeaMQTT.h>

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



unsigned long pumpOnTime, pumpOffTime;
bool pump = false;

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.printf("Serial speed: %d\n", SERIAL_SPEED);    
  
  pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  
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
  iTeaMQTT.publish("itea:pump:pub", "+");
  Serial.printf("Pump on at %d\n", pumpOnTime);
  digitalWrite(RELAY_PIN, LOW);
  pump = true;
  return ITEA_STATE_RUN;
}

uint8_t pumpOffHandler(uint8_t state, void *params ...) {
  Serial.printf("Countdown: %d\n", millis() - pumpOffTime);
  if ((millis() - pumpOffTime) > DP_DELAY) {
    iTeaMQTT.publish("itea:pump:pub", "-");
    Serial.printf("Pump off at %d\n", pumpOffTime); 
    digitalWrite(RELAY_PIN, HIGH);
  }
  return ITEA_STATE_RUN;
}

uint8_t runHandler(uint8_t state, void *params ...) {
  if (WL_CONNECTED == iTeaWiFi.connect()) {
    delay(DP_INTERVAL);
    if (LOW == digitalRead(WATER_SENSOR_PIN)) {
      pumpOnTime = millis();
      return DP_PUMPON;
    } else if (pump == true) {
      pump = false;
      pumpOffTime = millis();
      Serial.printf("Off singal at %d\n", pumpOffTime);
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
  iTeaMQTT.init(&iTeaConfig);
  uint8_t r = iTeaSetup.setup();
  if (r != ITEA_STATE_SETUP) {
    iTeaWiFi.connect();
    iTeaMQTT.subscribe("itea:pump:sub", callback);
    //r = iTeaMQTT.setup();
    if (r != ITEA_STATE_MQTT_CONNECT) {
      Serial.printf("MQTT Connection Error %d\n", iTeaMQTT.state());
    }
  } 
  return ITEA_STATE_RUN;  
}

uint8_t setupHandler(uint8_t state, void *params ...) {  
  return iTeaSetup.loop();
}

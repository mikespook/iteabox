#define SERIAL_SPEED 115200

// pins
#define PIN_PAW_SWITCH 4 // GPIO4, D4
#define PIN_HOLDER 16 // GPIO16, D2
// state
#define PF_HOLDER_RELEASE 50
// config
#define PF_INTERVAL 100
#define PF_HOLDER_DELAY 3000
#define PF_STEP_INTERVAL 5
#define PF_HOLDER_BEGIN 0
#define PF_HOLDER_END 180
#define PF_HOLDER_STEP 1

#include <Servo.h>
#include <iTeaHandler.h>
#include <iTeaConfig.h>
#include <iTeaSetup.h>
#include <iTeaWiFi.h>
#include <iTeaMQTT.h>

Servo holder;

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.printf("Serial speed: %d\n", SERIAL_SPEED);    
  
  pinMode(PIN_PAW_SWITCH, INPUT_PULLUP);
  holder.attach(PIN_HOLDER);
  
  Serial.printf("Load Config ... ");
  iTeaConfig.load();  
  Serial.println("Done");
  Serial.printf("Init Handler ... ");
  iTeaHandler.setup();
  iTeaHandler.add(ITEA_STATE_INIT, initHandler);
  iTeaHandler.add(ITEA_STATE_SETUP, setupHandler);
  iTeaHandler.add(ITEA_STATE_RUN, runHandler);
  iTeaHandler.add(PF_HOLDER_RELEASE, feedHandler);  
  Serial.println("Done");
}

void loop() {
  iTeaHandler.loop(NULL);
}

uint8_t feedHandler(uint8_t state, void *params ...) {
  Serial.printf("Holder open at %d\n", millis());
  for (int pos = PF_HOLDER_BEGIN; pos <= PF_HOLDER_END; pos += PF_HOLDER_STEP) {
    holder.write(pos);
    delay(PF_STEP_INTERVAL);
  }
  delay(PF_HOLDER_DELAY);
  for (int pos = PF_HOLDER_END; pos >= PF_HOLDER_BEGIN; pos -= PF_HOLDER_STEP) {
    holder.write(pos);
    delay(PF_STEP_INTERVAL);
  }
  iTeaMQTT.publish("itea:petsfeeder:pub", "+");
  return ITEA_STATE_RUN;
}

uint8_t runHandler(uint8_t state, void *params ...) {
  if (WL_CONNECTED == iTeaWiFi.connect()) {
    delay(PF_INTERVAL);
    if (HIGH == digitalRead(PIN_PAW_SWITCH)) {
      return PF_HOLDER_RELEASE;
    }
    iTeaMQTT.loop();
  }
  return ITEA_STATE_RUN; 
}

void callback(const char* topic, const uint8_t* payload, unsigned int) {
  iTeaHandler.call(PF_HOLDER_RELEASE, (void *)topic, (void *)payload);
}

uint8_t initHandler(uint8_t state, void *params ...) {  
  iTeaSetup.init(&iTeaConfig);
  iTeaWiFi.init(&iTeaConfig);
  iTeaMQTT.init(&iTeaConfig);
  iTeaMQTT.subscribe("itea:petsfeeder:sub", callback);
  return iTeaSetup.setup();  
}

uint8_t setupHandler(uint8_t state, void *params ...) {  
  return iTeaSetup.loop();
}

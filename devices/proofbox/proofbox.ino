#include <iTeaHandler.h>
#include <iTeaConfig.h>
#include <DHTesp.h>

#define SERIAL_SPEED 115200

#define ITEA_DHT_PIN	14  // D5
#define ITEA_BUTTON		12  // D6
#define ITEA_RELAY    13  // D7
#define ITEA_BUZZER   15  // D8
#define OFF LOW
#define ON HIGH

#define NOTE_C 262
#define NOTE_D 294
#define NOTE_E 330
#define NOTE_F 349
#define NOTE_G 392
#define NOTE_A 440
#define NOTE_B 494
#define TONE_SPEED 200

#define TEMP_MIN_1 26
#define TEMP_MAX_1 28

#define TEMP_MIN_2 36
#define TEMP_MAX_2 38

DHTesp dht;
int proofStep = 0;
int buttonPos = HIGH;
int relayState = OFF;
int lastOn = 0;
int lastOff = 0;

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.printf("Serial speed: %d\n", SERIAL_SPEED); 

  iTeaHandler.setup();
  iTeaHandler.add(ITEA_STATE_INIT, initHandler);
  iTeaHandler.add(ITEA_STATE_RUN, runHandler);
  heatOn();
}

void loop() {
  iTeaHandler.loop(NULL);
}


void heatOn() {
  if (relayState == OFF) {
    onBuz();
    relayState = ON;  
  }
  rawOn();
}

void rawOn() {
  digitalWrite(ITEA_RELAY, HIGH);
  lastOn = millis();
}

void heatOff() {
  if (relayState == ON) {
    offBuz();
    relayState = OFF;  
  }  
  rawOff();
}

void rawOff() {
  digitalWrite(ITEA_RELAY, LOW);
  lastOff = millis();  
}

uint8_t runHandler(uint8_t state, void *params ...) {
  if (digitalRead(ITEA_BUTTON) == LOW) {
    buttonPos = LOW;
    return ITEA_STATE_RUN; 
  }

  if (buttonPos == LOW) {
    buttonPos = HIGH;
    proofStep++;
    if (proofStep > 2) {
      proofStep = 0;  
    }
    switch(proofStep) {
      case 0:
      offBuz();
      break;
      case 1:
      step1Buz();
      break;
      case 2:
      step2Buz();
      break;  
    }
    return ITEA_STATE_RUN;    
  }

  delay(dht.getMinimumSamplingPeriod());
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  switch(proofStep) {
    case 1:
    Serial.println("Step 1");     
    proof1(humidity, temperature);
    break;
    case 2:
    Serial.println("Step 2");    
    proof2(humidity, temperature);
    break;
    default:
    Serial.println("Off");  
    heatOff();
    break;
  }
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 2);
  Serial.print("\t\t");
  Serial.println(temperature, 2);
  return ITEA_STATE_RUN; 
}

bool check(int last, float temperature, float target) {
  int now = millis();
  float t = target - temperature;
  t = log(-t + 11) * 30;
  if (t < 0) {
	t = 0;	  
  }
  return (now - last) > t * 1000;
}

void proof1(float humidity, float temperature) {  
  if (temperature < TEMP_MIN_1) {
    if (check(lastOn, temperature, TEMP_MIN_1)) {
      heatOn();
    } else {
      rawOff();  
    }
    return;
  }
  if (temperature >= TEMP_MAX_1) {
    overBuz();
    return; 
  }
  heatOff();
  return; 
}

void proof2(float humidity, float temperature) {
  if (temperature < TEMP_MIN_2) {
    if (check(lastOn, temperature, TEMP_MIN_2)) {
      heatOn();
    } else {
      rawOff();  
    }
    return; 
  }
  if (temperature >= TEMP_MAX_2) {
    overBuz();    
    return; 
  }
  heatOff();
  return; 
}


uint8_t initHandler(uint8_t state, void *params ...) {  
  Serial.println("Init...");
  dht.setup(ITEA_DHT_PIN, DHTesp::DHT11);
  pinMode(ITEA_RELAY, OUTPUT);
  pinMode(ITEA_BUZZER, OUTPUT);
  pinMode(ITEA_BUTTON, INPUT_PULLUP);
  heatOff();
  return ITEA_STATE_RUN;
}

void step1Buz() {
  tone(ITEA_BUZZER, NOTE_C, TONE_SPEED);
  delay(TONE_SPEED);  
  tone(ITEA_BUZZER, NOTE_E, TONE_SPEED);
  delay(TONE_SPEED);  
  tone(ITEA_BUZZER, NOTE_G, TONE_SPEED);  
}

void step2Buz() {
  tone(ITEA_BUZZER, NOTE_D, TONE_SPEED);
  delay(TONE_SPEED);  
  tone(ITEA_BUZZER, NOTE_F, TONE_SPEED);
  delay(TONE_SPEED);  
  tone(ITEA_BUZZER, NOTE_A, TONE_SPEED);  
}

void onBuz() {
  tone(ITEA_BUZZER, NOTE_C, TONE_SPEED);
  delay(TONE_SPEED);  
  tone(ITEA_BUZZER, NOTE_G, TONE_SPEED);
}

void offBuz() {
  tone(ITEA_BUZZER, NOTE_G, TONE_SPEED);
  delay(TONE_SPEED);  
  tone(ITEA_BUZZER, NOTE_C, TONE_SPEED);
}

void overBuz() {
  tone(ITEA_BUZZER, NOTE_A, TONE_SPEED); 
  delay(TONE_SPEED);
}

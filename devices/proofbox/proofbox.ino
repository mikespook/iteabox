#include <iTeaHandler.h>
#include <iTeaConfig.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

// #define DEBUG true

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

#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define LCD_BACKLIGHT_HOLDING 10000

#define BTN_HOLDING 1600
#define BTN_HOLDING_DELAY 100

LiquidCrystal_I2C lcd(0x3F, LCD_COLUMNS, LCD_ROWS);

DHTesp dht;
int proofStep = 0;
int buttonPos = HIGH;
int buttonPressed = 0;
int buttonHolding = 0;
int lastOn = 0;
int lastOff = 0;
int errorCount = 0;
int dhtWaiting = 0;

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.printf("Serial speed: %d\n", SERIAL_SPEED); 
  
  iTeaHandler.setup();
  iTeaHandler.add(ITEA_STATE_INIT, initHandler);
  iTeaHandler.add(ITEA_STATE_RUN, runHandler);
}

void loop() {
  iTeaHandler.loop(NULL);
}

void heatOn() {
  digitalWrite(ITEA_RELAY, HIGH);
  lastOn = millis();
}

void heatOff() {
  digitalWrite(ITEA_RELAY, LOW);
  lastOff = millis();  
}

uint8_t runHandler(uint8_t state, void *params ...) {
  if (buttonHolding <= BTN_HOLDING && digitalRead(ITEA_BUTTON) == LOW) {    
    lcd.backlight();
    buttonPos = LOW;
    buttonPressed = millis();    
    buttonHolding+=BTN_HOLDING_DELAY;
    buttonHoldingProgress(buttonHolding);
    delay(BTN_HOLDING_DELAY);
    return ITEA_STATE_RUN; 
  }
  if (buttonHolding > BTN_HOLDING && buttonPos == LOW) {
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
    buttonHolding = 0;
    return ITEA_STATE_RUN;    
  }
  buttonHolding = 0;
  if (millis() - buttonPressed > LCD_BACKLIGHT_HOLDING) {
    lcd.noBacklight();  
  }
  if(millis() - dhtWaiting > dht.getMinimumSamplingPeriod()){    
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    String stepText;
    switch(proofStep) {
      case 1:
      stepText = String(TEMP_MIN_1) + "-" + String(TEMP_MAX_1) + String((char)0xDF) + "C Proof #1";
      proof1(humidity, temperature);
      break;
      case 2:
      stepText = String(TEMP_MIN_2) + "-" + String(TEMP_MAX_2) + String((char)0xDF) + "C Proof #2";
      proof2(humidity, temperature);
      break;
      default:
      stepText = "Off";
      heatOff();
      break;
    }
    buttonPos = HIGH;
    printLCD(humidity, temperature, stepText);
    #ifdef DEBUG
    printSerial(humidity, temperature, stepText);
    #endif
    dhtWaiting = millis();
  }
  return ITEA_STATE_RUN; 
}

bool check(int last, float temperature, float target) {
  int now = millis();
  float t = target - temperature;
  t = log(-t + 11) * 30;
  if (t < 0) {
	t = 0;	  
  } 
  #ifdef DEBUG
  Serial.printf("Time: %d Threshold: %d\n", now - last, t * 1000);
  #endif
  return (now - last) > t * 1000;
}

void proof1(float humidity, float temperature) {  
  if (temperature < TEMP_MIN_1) {
    if (check(lastOn, temperature, TEMP_MIN_1)) {
      heatOn();
    } else {
      heatOff();
    }
    return;
  }
  if (temperature >= TEMP_MAX_1) {
    overBuz();
  }
  heatOff();
  return; 
}

void proof2(float humidity, float temperature) {
  if (temperature < TEMP_MIN_2) {
    if (check(lastOn, temperature, TEMP_MIN_2)) {
      heatOn();
    } else {
      heatOff();  
    }
    return; 
  }
  if (temperature >= TEMP_MAX_2) {
    overBuz();
  }
  heatOff();
  return; 
}


uint8_t initHandler(uint8_t state, void *params ...) {  
  #ifdef DEBUG
  Serial.println("Init...");
  #endif

  lcd.init();
  lcd.backlight();
  
  dht.setup(ITEA_DHT_PIN, DHTesp::DHT11);
  pinMode(ITEA_RELAY, OUTPUT);
  pinMode(ITEA_BUZZER, OUTPUT);
  pinMode(ITEA_BUTTON, INPUT_PULLUP);
  heatOn();
  heatOff();
  onBuz();
  dhtWaiting = buttonPressed = millis();   
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

void printLCD(float humidity, float temperature, String stepText) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(stepText);
  lcd.setCursor(0,1);
  if (isnan(temperature)) {
    lcd.print("--.--");
    errorCount++;
    if (errorCount > 5) {
      offBuz();
      errorCount = 0;
    }
  } else {
    lcd.print(temperature);
     errorCount = 0;
  }
  lcd.print((char)0xDF);
  lcd.print("C/");
  if (isnan(humidity)) {
    lcd.print("--.--");
  } else {
    lcd.print(humidity);
  }
  lcd.print("%");
}

void buttonHoldingProgress(int progress) {
  if (progress == BTN_HOLDING_DELAY) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Holding...");   
  }  
  int c = progress / BTN_HOLDING_DELAY % LCD_COLUMNS - 1;  
  lcd.setCursor(c, 1);
  lcd.print((char)0xFF); 
}

void printSerial(float humidity, float temperature, String stepText) {
  Serial.print(stepText);
  Serial.print("\tDHT Status: ");
  Serial.print(dht.getStatusString());
  Serial.print("\t Humidity: ");
  Serial.print(humidity, 2);
  Serial.print("\tTemperature: ");
  Serial.println(temperature, 2);  
}

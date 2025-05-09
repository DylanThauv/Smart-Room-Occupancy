// Smart Room Occupancy Monitor (Modular Version)
// Refactored to modularize logic into separate components
// Uses: 2x Ultrasonic Sensors, LCD (I2C), Button, Buzzer
// Goal: Count people in/out, enforce limit, toggle alarm with button

#include <y> // Replace with: Wire.h (I2C communication for LCD)
#include <y> // Replace with: LiquidCrystal_I2C.h (LCD display library)

#define TRIG_IN x     // Trigger pin for entrance ultrasonic sensor
#define ECHO_IN x     // Echo pin for entrance ultrasonic sensor
#define TRIG_OUT x    // Trigger pin for exit ultrasonic sensor
#define ECHO_OUT x    // Echo pin for exit ultrasonic sensor
#define BUTTON_PIN x  // Pin connected to the alarm toggle button
#define BUZZER_PIN x  // Pin connected to the buzzer output

// Global state variables
int occupancy = 0;                      // Current number of people
const int MAX_OCCUPANCY = 5;           // Max allowed people in room
bool alarmTriggered = false;           // Alarm status
bool lastButtonState = HIGH;           // For detecting button presses

LiquidCrystal_I2C lcd(0x27, 16, 2);     // LCD object with I2C address

// ----------- SENSOR MODULE -----------
void setupUltrasonicSensors() {
  pinMode(TRIG_IN, OUTPUT);
  pinMode(ECHO_IN, INPUT);
  pinMode(TRIG_OUT, OUTPUT);
  pinMode(ECHO_OUT, INPUT);
}

float readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void checkEntryExit() {
  float distIn = readDistance(TRIG_IN, ECHO_IN);
  float distOut = readDistance(TRIG_OUT, ECHO_OUT);

  if (distIn < 20) {
    occupancy++;
    delay(1500);
  }
  if (distOut < 20 && occupancy > 0) {
    occupancy--;
    delay(1500);
  }
}

// ----------- DISPLAY MODULE -----------
void setupDisplay() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Occupancy: 0");
  lcd.setCursor(0, 1);
  lcd.print("Status: OK");
}

void updateDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("Occupancy:     ");
  lcd.setCursor(11, 0);
  lcd.print(occupancy);

  lcd.setCursor(0, 1);
  lcd.print("Status:       ");
  lcd.setCursor(8, 1);
  lcd.print(alarmTriggered ? "ALARM" : "OK");
}

// ----------- ALARM MODULE -----------
void setupAlarmSystem() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void triggerAlarm() {
  digitalWrite(BUZZER_PIN, HIGH);
  alarmTriggered = true;
}

void disableAlarm() {
  digitalWrite(BUZZER_PIN, LOW);
  alarmTriggered = false;
}

void toggleAlarm() {
  if (alarmTriggered) disableAlarm();
  else triggerAlarm();
}

void handleButtonPress() {
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH) {
    toggleAlarm();
    delay(200);
  }
  lastButtonState = buttonState;
}

// ----------- ARDUINO STANDARD FUNCTIONS -----------
void setup() {
  setupUltrasonicSensors();
  setupDisplay();
  setupAlarmSystem();
  Serial.begin(9600);
}

void loop() {
  checkEntryExit();

  if (occupancy >= MAX_OCCUPANCY) {
    triggerAlarm();
  }

  handleButtonPress();
  updateDisplay();
  delay(200);
}

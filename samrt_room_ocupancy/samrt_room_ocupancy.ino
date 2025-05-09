// Smart Room Occupancy Monitor
// Uses: 2x Ultrasonic Sensors, LCD (I2C), Button, Buzzer
// Goal: Count people in/out, enforce limit, allow alarm control with single button press

#include <y> // Replace with: Wire.h (I2C communication for LCD)
#include <y> // Replace with: LiquidCrystal_I2C.h (LCD display library)

#define TRIG_IN x   // Trigger pin for entrance ultrasonic sensor
#define ECHO_IN x   // Echo pin for entrance ultrasonic sensor
#define TRIG_OUT x  // Trigger pin for exit ultrasonic sensor
#define ECHO_OUT x  // Echo pin for exit ultrasonic sensor

#define BUTTON_PIN x   // Pin connected to the alarm toggle button
#define BUZZER_PIN x   // Pin connected to the buzzer output

// Create LCD object with I2C address 0x27 and dimensions 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

int occupancy = 0;                      // Track number of people currently in the room
const int MAX_OCCUPANCY = 5;           // Define maximum room capacity
bool alarmTriggered = false;           // Alarm status flag (true if alarm is on)
bool lastButtonState = HIGH;           // Used for detecting button press transitions

// Function to read distance in cm from an ultrasonic sensor
float readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);           // Ensure trigger pin is low
  digitalWrite(trigPin, HIGH);          // Send 10 microsecond pulse to trigger
  digitalWrite(trigPin, LOW);           // End of trigger pulse

  float duration = pulseIn(echoPin, HIGH);       // Read echo pulse duration
  float distance = duration * 0.034 / 2;         // Convert to distance in cm
  return distance;
}

void setup() {
  // Configure ultrasonic sensor pins
  pinMode(TRIG_IN, OUTPUT);
  pinMode(ECHO_IN, INPUT);
  pinMode(TRIG_OUT, OUTPUT);
  pinMode(ECHO_OUT, INPUT);

  // Configure button and buzzer
  pinMode(BUTTON_PIN, INPUT_PULLUP);     // Use internal pull-up for button
  pinMode(BUZZER_PIN, OUTPUT);           // Set buzzer pin as output

  // Initialize LCD
  lcd.init();                            // Start LCD
  lcd.backlight();                       // Turn on LCD backlight
  lcd.setCursor(0, 0);                   // Set to first row, first column
  lcd.print("Occupancy: 0");             // Initial display for occupancy
  lcd.setCursor(0, 1);                   // Set to second row
  lcd.print("Status: OK");               // Initial display for status

  // Start serial communication for debugging
  Serial.begin(9600);
}

// Function to update the LCD with current occupancy and alarm status
void updateDisplay() {
  lcd.setCursor(0, 0);                          // Set cursor to first row
  lcd.print("Occupancy:     ");                // Clear line with padding
  lcd.setCursor(11, 0);                         // Set cursor for number
  lcd.print(occupancy);                         // Print current occupancy

  lcd.setCursor(0, 1);                          // Set cursor to second row
  lcd.print("Status:       ");                 // Clear line with padding
  lcd.setCursor(8, 1);                          // Set cursor for status
  lcd.print(alarmTriggered ? "ALARM" : "OK");  // Show alarm state
}

void loop() {
  // Measure distance from both sensors
  float distIn = readDistance(TRIG_IN, ECHO_IN);   // Distance at entrance
  float distOut = readDistance(TRIG_OUT, ECHO_OUT); // Distance at exit

  // Person detected entering if close to entrance sensor
  if (distIn < 20) {
    occupancy++;                                  // Increase count
    delay(1500);                                  // Debounce to avoid double count
  }

  // Person detected exiting if close to exit sensor
  if (distOut < 20 && occupancy > 0) {
    occupancy--;                                  // Decrease count
    delay(1500);                                  // Debounce delay
  }

  // Automatically trigger alarm if occupancy exceeds limit
  if (occupancy >= MAX_OCCUPANCY) {
    triggerAlarm();
  }

  // Read button state to manually toggle alarm
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH) { // Detect press event
    toggleAlarm();                                      // Toggle alarm state
    delay(200);                                         // Debounce delay
  }
  lastButtonState = buttonState;                        // Save current button state

  updateDisplay();      // Refresh display with new info
  delay(200);           // Loop delay
}

// Function to activate alarm
void triggerAlarm() {
  digitalWrite(BUZZER_PIN, HIGH);   // Turn on buzzer
  alarmTriggered = true;            // Set alarm status flag
}

// Function to deactivate alarm
void disableAlarm() {
  digitalWrite(BUZZER_PIN, LOW);    // Turn off buzzer
  alarmTriggered = false;           // Clear alarm status flag
}

// Function to toggle alarm on/off
void toggleAlarm() {
  if (alarmTriggered) {
    disableAlarm();    // If alarm is on, turn it off
  } else {
    triggerAlarm();    // If alarm is off, turn it on
  }
}

// Smart Room Occupancy Monitor (Modular Version)
// Refactored to modularize logic into separate components
// Uses: 2x Ultrasonic Sensors, LCD (I2C), Button, Buzzer
// Goal: Count people in/out, enforce limit, toggle alarm with button

#include <Wire.h> // Replace with: Wire.h
#include <LiquidCrystal.h> //Replace with: LiquidCrystal.h (LCD display library)

// Define LCD pin connections 
const int RS = 12, E = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
// Create LCD object for standard 16x2 screen
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

// Define pin assignments
#define TRIG_IN 8     // Trigger pin for entrance ultrasonic sensor
#define ECHO_IN 9     // Echo pin for entrance ultrasonic sensor
#define TRIG_OUT 10    // Trigger pin for exit ultrasonic sensor
#define ECHO_OUT  13  // Echo pin for exit ultrasonic sensor
#define BUTTON_PIN 6  // Pin connected to the alarm toggle button
#define BUZZER_PIN 7  // Pin connected to the buzzer output

// Global state variables
int occupancy = 0;                      // Current number of people in the room
const int MAX_OCCUPANCY = 3;           // Maximum number of people allowed
bool alarmTriggered = false;           // Tracks whether the alarm is active
bool lastButtonState = HIGH;           // Stores previous state of the button for edge detection


// ----------- SENSOR MODULE -----------

// Initializes ultrasonic sensor pins
void setupUltrasonicSensors() {
  pinMode(TRIG_IN, OUTPUT);  // Set entrance trigger pin as output
  pinMode(ECHO_IN, INPUT);   // Set entrance echo pin as input
  pinMode(TRIG_OUT, OUTPUT); // Set exit trigger pin as output
  pinMode(ECHO_OUT, INPUT);  // Set exit echo pin as input
}

// Reads distance from ultrasonic sensor using trigger and echo pins
float readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);            // Ensure trigger is LOW
  delayMicroseconds(2);                  // Small delay for stability
  digitalWrite(trigPin, HIGH);           // Trigger ultrasonic burst
  delayMicroseconds(10);                 // Duration of burst
  digitalWrite(trigPin, LOW);            // Turn off trigger
  float duration = pulseIn(echoPin, HIGH); // Read echo duration
  return duration * 0.034 / 2;           // Convert time to distance in cm
}
void checkEntryExit() {
  // Read IN sensor
  long distIn = readDistance(TRIG_IN, ECHO_IN);
  delay(100); // Let echoes die out before triggering next sensor

  // Read OUT sensor
  long distOut = readDistance(TRIG_OUT, ECHO_OUT);

  Serial.print("Distance IN: ");
  Serial.print(distIn);
  Serial.print(" cm\t");

  Serial.print("Distance OUT: ");
  Serial.print(distOut);
  Serial.println(" cm");

  if (distIn < 20) {
    occupancy++;
    delay(1500); // Debounce: allow time for person to move through
  }

  if (distOut < 20 && occupancy > 0) {
    occupancy--;
    delay(1500);
  }
}




// Updates the content on the LCD
void updateDisplay() {
  lcd.clear();                      // clears the lcd to not overload
  lcd.setCursor(0, 0);             // Move cursor to start of first line
  lcd.print("Occupancy:     ");    // Clear existing number
  lcd.setCursor(11, 0);            // Move cursor to position to print number
  lcd.print(occupancy);           // Display current occupancy
  lcd.setCursor(0, 1);             // Move cursor to start of second line
  lcd.print("Status:       ");     // Clear existing status
  lcd.setCursor(8, 1);             // Move to status position
  lcd.print(alarmTriggered ? "ALARM" : "OK"); // Show alarm status
}

// ----------- ALARM MODULE -----------

// Initializes alarm (buzzer and button)
void setupAlarmSystem() {
  pinMode(BUZZER_PIN, OUTPUT);      // Set buzzer as output
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set button with internal pull-up resistor
}

// Activates the buzzer alarm
void triggerAlarm() {
  digitalWrite(BUZZER_PIN, HIGH);   // Turn on buzzer
  alarmTriggered = true;            // Mark alarm as active
}

// Deactivates the buzzer alarm
void disableAlarm() {
  digitalWrite(BUZZER_PIN, LOW);    // Turn off buzzer
  alarmTriggered = false;           // Mark alarm as inactive
}

// Toggles alarm state (on/off)
void toggleAlarm() {
  if (alarmTriggered) disableAlarm(); // If alarm is active, disable it
  else triggerAlarm();                // Otherwise, enable it
}

// Detects a button press and toggles the alarm
void handleButtonPress() {
  bool buttonState = digitalRead(BUTTON_PIN); // Read current button state
  if (buttonState == LOW && lastButtonState == HIGH) { // Check for new press
    toggleAlarm();                       // Toggle alarm state
    delay(200);                          // Debounce delay
  }
  lastButtonState = buttonState;         // Update last state
}

// ----------- ARDUINO STANDARD FUNCTIONS -----------

// Runs once at startup
void setup() {
  setupUltrasonicSensors();   // Configure ultrasonic pins
  setupAlarmSystem();         // Initialize alarm system
  Serial.begin(9600);         // Start serial communication
  lcd.begin(16,2);                // Initialize LCD
}

// Runs repeatedly
void loop() {

  checkEntryExit();           // Check for entries or exits
 if (occupancy >= MAX_OCCUPANCY) { // If over the limit
    triggerAlarm();           // Activate alarm
  }
  handleButtonPress();        // Listen for manual alarm toggle
  updateDisplay();            // Refresh LCD display
  delay(200);                 // Loop delay
  


}

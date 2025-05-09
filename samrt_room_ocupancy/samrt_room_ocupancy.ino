// Smart Room Occupancy Monitor (Modular Version)
// Refactored to modularize logic into separate components
// Uses: 2x Ultrasonic Sensors, LCD (I2C), Button, Buzzer
// Goal: Count people in/out, enforce limit, toggle alarm with button

#include <y> // Replace with: Wire.h (I2C communication for LCD)
#include <y> // Replace with: LiquidCrystal_I2C.h (LCD display library)

// Define pin assignments
#define TRIG_IN x     // Trigger pin for entrance ultrasonic sensor
#define ECHO_IN x     // Echo pin for entrance ultrasonic sensor
#define TRIG_OUT x    // Trigger pin for exit ultrasonic sensor
#define ECHO_OUT x    // Echo pin for exit ultrasonic sensor
#define BUTTON_PIN x  // Pin connected to the alarm toggle button
#define BUZZER_PIN x  // Pin connected to the buzzer output

// Global state variables
int occupancy = 0;                      // Current number of people in the room
const int MAX_OCCUPANCY = 5;           // Maximum number of people allowed
bool alarmTriggered = false;           // Tracks whether the alarm is active
bool lastButtonState = HIGH;           // Stores previous state of the button for edge detection

// Create LCD display object using I2C address 0x27, with 16 columns and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ----------- SENSOR MODULE -----------

// Initializes ultrasonic sensor pins
void setupUltrasonicSensors() {
  pinMode(TRIG_IN, OUTPUT);  // Set entrance trigger pin as output
  pinMode(ECHO_IN, INPUT);   // Set entrance echo pin as input
  pinMode(TRIG_OUT, OUTPUT); // Set exit trigger pin as output
  pinMode(ECHO_OUT, INPUT);  // Set exit echo pin as input
}

// Reads distance from ultrasonic sensor using trigger and echo pins
long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);            // Ensure trigger is LOW
  delayMicroseconds(2);                  // Small delay for stability
  digitalWrite(trigPin, HIGH);           // Trigger ultrasonic burst
  delayMicroseconds(10);                 // Duration of burst
  digitalWrite(trigPin, LOW);            // Turn off trigger
  long duration = pulseIn(echoPin, HIGH); // Read echo duration
  return duration * 0.034 / 2;           // Convert time to distance in cm
}

// Monitors both sensors to detect entry and exit
void checkEntryExit() {
  long distIn = readDistance(TRIG_IN, ECHO_IN);   // Measure distance at entrance
  long distOut = readDistance(TRIG_OUT, ECHO_OUT); // Measure distance at exit

  if (distIn < 20) {         // If object detected close at entrance
    occupancy++;             // Increase occupancy count
    delay(1500);             // Wait to avoid double count
  }
  if (distOut < 20 && occupancy > 0) { // If object detected close at exit
    occupancy--;             // Decrease occupancy count
    delay(1500);             // Wait to avoid double count
  }
}

// ----------- DISPLAY MODULE -----------

// Sets up the LCD display
void setupDisplay() {
  lcd.init();                // Initialize LCD
  lcd.backlight();           // Turn on backlight
  lcd.setCursor(0, 0);       // Set cursor to first line
  lcd.print("Occupancy: 0"); // Display initial occupancy
  lcd.setCursor(0, 1);       // Set cursor to second line
  lcd.print("Status: OK");   // Display initial status
}

// Updates the content on the LCD
void updateDisplay() {
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
  setupDisplay();             // Initialize display
  setupAlarmSystem();         // Initialize alarm system
  Serial.begin(9600);         // Start serial communication
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

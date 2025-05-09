//Dylan Thauvette and Stephen Tudor
//dawson college
//Smart Room Ocupancy Sensor

#define TRIG_IN x   // Trigger pin for entrance sensor
#define ECHO_IN x   // Echo pin for entrance sensor
#define TRIG_OUT x  // Trigger pin for exit sensor
#define ECHO_OUT x  // Echo pin for exit sensor

// This function reads distance from an ultrasonic sensor
float readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);       // Ensure trigger pin is LOW
  delayMicroseconds(2);             // Short delay to stabilize
  digitalWrite(trigPin, HIGH);      // Send 10µs pulse to trigger
  delayMicroseconds(10);            // Keep HIGH for 10µs
  digitalWrite(trigPin, LOW);       // End trigger pulse

  float duration = pulseIn(echoPin, HIGH);         // Read echo duration
  float distance = duration * 0.034 / 2;           // Convert to cm
  return distance;                                // Return distance
}
void updateDisplay() {
  lcd.setCursor(0, 0);                            // Move to first line
  lcd.print("Occupancy:     ");                  // Clear previous text
  lcd.setCursor(11, 0);                           // Move to number position
  lcd.print(occupancy);                           // Show current count

  lcd.setCursor(0, 1);                            // Move to second line
  lcd.print("Status:       ");                   // Clear previous status
  lcd.setCursor(8, 1);                            // Position for status
  lcd.print(alarmTriggered ? "ALARM" : "OK");     // Show "ALARM" or "OK"
}


void loop() {

  float distIn = readDistance(TRIG_IN, ECHO_IN);
  float distOut = readDistance(TRIG_OUT, ECHO_OUT);

// Person entering
    if (distIn < 20) {
    occupancy++;
      delay(1500); // Debounce delay to avoid double count
}

// Person exiting
      if (distOut < 20 && occupancy > 0) {
        occupancy--;
         delay(1500); // Debounce delay to avoid double count
}


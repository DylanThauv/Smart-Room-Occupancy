//Dylan Thauvette and Stephen Tudor
//dawson college
//

#define TRIG_IN x   // Trigger pin for entrance sensor
#define ECHO_IN x   // Echo pin for entrance sensor
#define TRIG_OUT x  // Trigger pin for exit sensor
#define ECHO_OUT x  // Echo pin for exit sensor

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
}

// Pins where the LEDs are connected
const int ledPins[] = {10, 11, 12}; // LED pins: 10, 11, 12
const int numLeds = 3; // Number of LEDs

// Force thresholds for each LED
const float thresholds[] = {1.0, 0.5}; // Thresholds for pin 10 and 11 in pounds

// Variables to simulate force changing over time
float forceInPounds = 0.0;  // Starting force
float maxForce = 1.5;       // Maximum force in pounds
int duration = 5000;       // Duration of 10 seconds in milliseconds
int steps = 150;            // Number of steps (0.01 lb increments)
int delayTime = duration / steps; // Time between each step
int endDelayTime = 3000;    // Time to keep LEDs on after max force (3 seconds)
int flashDuration = 2000;   // Duration of LED flash (5 seconds)
int flashInterval = 100;    // Flash interval in milliseconds (rapid flashing)

void setup() {
  // Set all the LED pins as OUTPUT
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  
  // Initialize serial communication for debugging (optional)
  Serial.begin(9600);
}

void loop() {
  // Simulate force incrementing over time
  for (forceInPounds = 0.0; forceInPounds <= maxForce; forceInPounds += 0.01) {
    // Print the current force value to the serial monitor
    Serial.print("Force: ");
    Serial.println(forceInPounds);

    // Turn on LEDs based on the current force
    if (forceInPounds >= thresholds[0]) {
      digitalWrite(ledPins[0], HIGH); // Turn on LED on pin 10 if force is >= 1 lb
    } else {
      digitalWrite(ledPins[0], LOW);  // Turn off LED on pin 10 if force < 1 lb
    }
    
    if (forceInPounds >= thresholds[1]) {
      digitalWrite(ledPins[1], HIGH); // Turn on LED on pin 11 if force is >= 0.5 lb
    } else {
      digitalWrite(ledPins[1], LOW);  // Turn off LED on pin 11 if force < 0.5 lb
    }

    // Ensure pin 12 LED is on whenever force is >= 0
    digitalWrite(ledPins[2], HIGH);

    // Wait for the next step in the force increment
    delay(delayTime);  // Delay to simulate gradual force increase
  }
  
  // Keep the LEDs on for a few seconds after reaching max force
  delay(endDelayTime);  // Delay for 3 seconds (3000 ms)

  // Flash all LEDs for 5 seconds
  unsigned long startFlashTime = millis(); // Record the start time
  while (millis() - startFlashTime < flashDuration) {
    // Turn all LEDs on
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], HIGH);
    }
    delay(flashInterval);
    
    // Turn all LEDs off
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], LOW);
    }
    delay(flashInterval);
  }

  // After flashing, ensure pin 12 LED is on
  digitalWrite(ledPins[0], LOW); // Turn off LED on pin 10
  digitalWrite(ledPins[1], LOW); // Turn off LED on pin 11
  digitalWrite(ledPins[2], HIGH); // Turn LED on pin 12 back on

  // Optionally restart the simulation (uncomment if needed)
  // delay(2000); // Wait before restarting (optional)
  // forceInPounds = 0; // Reset force
}



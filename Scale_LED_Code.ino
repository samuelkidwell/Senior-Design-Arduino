#include "HX711.h"

// Pin configuration for the load cell
const int LOADCELL_DOUT_PIN = 3;  // DT pin of HX711
const int LOADCELL_SCK_PIN = 2;   // SCK pin of HX711

// Pin configuration for the LEDs
const int LED_1KG_PIN = 11;  // LED for weight > 1kg
const int LED_2KG_PIN = 10;  // LED for weight > 2kg
const int LED_0KG_PIN = 12;  // LED for weight > 0kg (absolute value)

// HX711 initialization
HX711 scale;

// Calibration factor variable
float calibration_factor;  // Start with no default calibration factor
float threshold = 0.05;  // Threshold to consider as zero (±0.05 kg)

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Set up the scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Set LED pins as outputs
  pinMode(LED_1KG_PIN, OUTPUT);
  pinMode(LED_2KG_PIN, OUTPUT);
  pinMode(LED_0KG_PIN, OUTPUT);

  // Initialize LEDs to be off
  digitalWrite(LED_1KG_PIN, LOW);
  digitalWrite(LED_2KG_PIN, LOW);
  digitalWrite(LED_0KG_PIN, LOW);

  // Calibrate the scale (this will replace any old calibration factor)
  calibrate_scale();

  // Tare the scale after calibration to ensure zero with no load
  scale.tare();
  Serial.println("Scale tared again to reset to zero after calibration.");
}

void loop() {
  // Check if the scale is ready to read
  if (scale.is_ready()) {
    // Read the weight value from the load cell
    float weight = scale.get_units(10);  // Get the average of 10 readings
    float abs_weight = abs(weight);  // Use the absolute value of the weight

    // Apply a small threshold to ignore minor fluctuations (±0.05 kg)
    if (abs_weight < threshold) {
      weight = 0.0;  // Consider values within threshold as zero
    }

    // Display the weight on the Serial Monitor
    Serial.print("Recorded Load: ");
    Serial.print(weight, 2);  // Display with two decimal places
    Serial.println(" kg");

    // LED Control logic:
    // If absolute weight > 0 kg, turn on LED on pin 12; else, turn it off
    if (abs_weight > 0.0) {
      digitalWrite(LED_0KG_PIN, HIGH);
    } else {
      digitalWrite(LED_0KG_PIN, LOW);
    }

    // If absolute weight > 1 kg, turn on LED on pin 11; else, turn it off
    if (abs_weight > 1.0) {
      digitalWrite(LED_1KG_PIN, HIGH);
    } else {
      digitalWrite(LED_1KG_PIN, LOW);
    }

    // If absolute weight > 2 kg, turn on LED on pin 10; else, turn it off
    if (abs_weight > 2.0) {
      digitalWrite(LED_2KG_PIN, HIGH);
    } else {
      digitalWrite(LED_2KG_PIN, LOW);
    }
  } else {
    Serial.println("Scale not ready.");
  }

  // Update every half a second
  delay(500);
}

void calibrate_scale() {
  // Prompt the user for calibration
  Serial.println("Place the scale on a stable surface and ensure no load is applied.");
  Serial.println("Press any key and Enter to tare the scale (set to zero)...");

  // Wait for user to press a key to tare
  while (Serial.available() == 0) {
    // Wait for input
  }
  Serial.read();  // Clear input buffer

  // Tare the scale to reset the baseline
  scale.tare();
  Serial.println("Scale tared.");

  // Ask the user to place a known weight
  Serial.println("Place a known weight on the scale (e.g., 0.4 kg), then press any key and Enter.");

  // Wait for user to press a key to proceed
  while (Serial.available() == 0) {
    // Wait for input after placing weight
  }
  Serial.read();  // Clear input buffer

  // Ask for the known weight value
  Serial.println("Enter the known weight in kg (e.g., 0.40):");

  // Wait for the user to input the known weight
  while (Serial.available() == 0) {
    // Wait for weight input
  }

  // Read the known weight input
  float known_weight = Serial.parseFloat();
  Serial.print("You entered: ");
  Serial.print(known_weight, 2);
  Serial.println(" kg.");

  // Get a stable raw reading from the load cell
  Serial.println("Please keep the weight on the scale for a few seconds for stabilization.");
  delay(3000);  // Wait for a few seconds for the weight to stabilize
  long reading = scale.get_units(10);  // Get an average of 10 readings
  Serial.print("Raw reading: ");
  Serial.println(reading);

  // Calculate the new calibration factor
  calibration_factor = reading / known_weight;
  Serial.print("New calibration factor: ");
  Serial.println(calibration_factor, 5);

  // Apply the new calibration factor (overwrite any old factor)
  scale.set_scale(calibration_factor);
  Serial.println("Calibration complete! The scale is now ready.");
}

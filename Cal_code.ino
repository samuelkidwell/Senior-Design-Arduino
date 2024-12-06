#include "HX711.h"

// Pin configuration
const int LOADCELL_DOUT_PIN = 3;  // DT pin of HX711
const int LOADCELL_SCK_PIN = 2;   // SCK pin of HX711

// HX711 initialization
HX711 scale;

float calibration_factor = 1.0;  // Initial calibration factor (this will be adjusted)

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Set up the scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Initializing the scale...");
  delay(1000);

  // Tare the scale to set the baseline
  Serial.println("Taring the scale...");
  scale.tare();

  Serial.println("Place a known weight on the scale and enter its weight in kg.");
}

void loop() {
  // Check if the scale is ready
  if (scale.is_ready()) {
    // Get raw reading from the scale
    long raw_value = scale.read_average(10); // Average of 10 readings for stability
    Serial.print("Raw Value: ");
    Serial.println(raw_value);
  } else {
    Serial.println("Scale not ready.");
  }

  // Check for user input in the Serial Monitor
  if (Serial.available() > 0) {
    String input = Serial.readString();
    float known_weight = input.toFloat();

    if (known_weight > 0) {
      // Calculate the calibration factor
      calibration_factor = scale.read_average(10) / known_weight;
      Serial.print("Calibration Factor: ");
      Serial.println(calibration_factor, 4);

      // Apply the calibration factor
      scale.set_scale(calibration_factor);

      Serial.println("Calibration complete. Use this factor in your final code.");
      while (true);  // Stop the loop to prevent further changes
    }
  }

  delay(500);
}


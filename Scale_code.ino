#include "HX711.h"

// Pin configuration
const int LOADCELL_DOUT_PIN = 3;  // DT pin of HX711
const int LOADCELL_SCK_PIN = 2;   // SCK pin of HX711

// HX711 initialization
HX711 scale;

// Use the calibration factor obtained during calibration
float calibration_factor = 4062.5422;

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Set up the scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Set the calibration factor
  scale.set_scale(calibration_factor);

  // Tare the scale to set the zero point (no load)
  Serial.println("Taring the scale...");
  scale.tare();  // Reset the scale to 0 without any weight applied

  Serial.println("Scale is ready.");
}

void loop() {
  // Check if scale is ready to read
  if (scale.is_ready()) {
    // Read the weight value from the load cell
    float weight = scale.get_units(10);  // Get the average of 10 readings
    float weight2 = weight*0.04421;
    
    // Display the weight on the Serial Monitor
    Serial.print("Recorded Load: ");
    Serial.print(weight2, 3);  // Display with two decimal places
    Serial.println(" kg");
  } else {
    Serial.println("Scale not ready.");
  }

  // Update every half a second
  delay(500);
}

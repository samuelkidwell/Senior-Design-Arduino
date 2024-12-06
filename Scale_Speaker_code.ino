#include "HX711.h"

// Pin configuration
const int LOADCELL_DOUT_PIN = 3;  // DT pin of HX711
const int LOADCELL_SCK_PIN = 2;   // SCK pin of HX711
const int amplifierPin = 4;       // Pin for the amplifier/speaker

// HX711 initialization
HX711 scale;

// Use the calibration factor obtained during calibration
float calibration_factor = 4062.5422;

// Set threshold for reward beeps
const float thresholdKg = 0.5;  // Set threshold to 5kg

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

  // Set amplifier pin as output
  pinMode(amplifierPin, OUTPUT);
}

void loop() {
  // Check if scale is ready to read
  if (scale.is_ready()) {
    // Read the weight value from the load cell
    float weight = scale.get_units(10);  // Get the average of 10 readings
    float weight2 = weight * 0.04421;    // Convert to kg if needed

    // Display the weight on the Serial Monitor
    Serial.print("Recorded Load: ");
    Serial.print(weight2, 3);  // Display with three decimal places
    Serial.println(" kg");

    // Check if the weight exceeds the threshold
    if (weight2 > thresholdKg) {
      // Play a sequence of upbeat beeps through the amplifier/speaker
      playRewardBeeps();
      delay(5000);  // Delay to avoid playing beeps repeatedly
    }
  } else {
    Serial.println("Scale not ready.");
  }

  // Update every half a second
  delay(500);
}

void playRewardBeeps() {
  // Play 3 short beeps to reward using the amplifier/speaker
  for (int i = 0; i < 3; i++) {
    tone(amplifierPin, 1000, 200);  // Send a 1000Hz signal to the amplifier for 200ms
    delay(300);                     // Delay between beeps
  }
}

#include "HX711.h"

// Pin configuration
const int LOADCELL_DOUT_PIN = 3;  // DT pin of HX711
const int LOADCELL_SCK_PIN = 2;   // SCK pin of HX711
const int amplifierPin = 4;       // Pin for the amplifier/speaker
const int vibrationMotorPin = 5;  // Pin to control the vibration motor through the transistor
const int ledPin0 = 10;           // LED pin for 0 kg threshold
const int ledPin05 = 11;          // LED pin for 0.5 kg threshold
const int ledPin1 = 12;           // LED pin for 1 kg threshold

// HX711 initialization
HX711 scale;

// Use the calibration factor obtained during calibration
float calibration_factor = 4062.5422;

// Set threshold for reward beeps and vibration
const float thresholdKg = 0.5;  // Set threshold to 0.5 kg

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

    // Set amplifier, vibration motor, and LED pins as output
    pinMode(amplifierPin, OUTPUT);
    pinMode(vibrationMotorPin, OUTPUT);
    pinMode(ledPin0, OUTPUT);
    pinMode(ledPin05, OUTPUT);
    pinMode(ledPin1, OUTPUT);

    // Turn off other LEDs initially
    digitalWrite(ledPin0, LOW);
    digitalWrite(ledPin05, LOW);
    digitalWrite(ledPin1, LOW);
}

void loop() {
    // Check if the scale is ready to read
    if (scale.is_ready()) {
        // Read the weight value from the load cell
        float weight = scale.get_units(10);  // Get the average of 10 readings
        float weightKg = weight * 0.04421;    // Convert to kg if needed

        // Display the weight on the Serial Monitor
        Serial.print("Recorded Load: ");
        Serial.print(weightKg, 3);  // Display with three decimal places
        Serial.println(" kg");

        // Check if the weight exceeds the threshold
        if (weightKg >= thresholdKg) {
            // Play a single beep through the amplifier/speaker
            tone(amplifierPin, 1000, 200);  // Send a 1000Hz signal to the amplifier for 200ms

            // Turn on the vibration motor
            digitalWrite(vibrationMotorPin, HIGH);
            delay(1000); // Activate the vibration motor for 1 second
            digitalWrite(vibrationMotorPin, LOW); // Turn off the vibration motor

            // Check for weight thresholds and update LEDs
            if (weightKg >= 1.0) {
                digitalWrite(ledPin1, HIGH);  // LED for 1 kg ON
            } else {
                digitalWrite(ledPin1, LOW);   // LED for 1 kg OFF
            }

            if (weightKg >= 0.5) {
                digitalWrite(ledPin05, HIGH); // LED for 0.5 kg ON
            } else {
                digitalWrite(ledPin05, LOW);  // LED for 0.5 kg OFF
            }

            // Always keep LED for 0 kg on
            digitalWrite(ledPin0, HIGH); // LED for 0 kg ON

        } else {
            // Ensure the vibration motor is turned off when weight is below threshold
            digitalWrite(vibrationMotorPin, LOW);

            // Turn off LEDs when below threshold
            digitalWrite(ledPin0, LOW); // LED for 0 kg OFF
            digitalWrite(ledPin05, LOW); // LED for 0.5 kg OFF
            digitalWrite(ledPin1, LOW); // LED for 1 kg OFF
        }

    } else {
        Serial.println("Scale not ready.");
    }

    // Update every half a second
    delay(500);
}

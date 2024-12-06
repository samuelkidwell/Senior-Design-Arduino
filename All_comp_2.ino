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

// Variables for non-blocking motor control
unsigned long motorStartTime = 0;
bool motorRunning = false;

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
        float weightKg = abs(weight * 0.04421);    // Conversion for correct weight, also keeps values positive

        // Display the weight on the Serial Monitor
        Serial.print("Recorded Load: ");
        Serial.print(weightKg, 3);  // Display with three decimal places
        Serial.println(" kg");

        // Check if the weight exceeds the threshold and the motor is not already running
        if (weightKg >= thresholdKg && !motorRunning) {
            // Play a single beep through the amplifier/speaker
            tone(amplifierPin, 1000, 200);  // Send a 1000Hz signal to the amplifier for 200ms

            // Turn on the vibration motor
            digitalWrite(vibrationMotorPin, HIGH);
            motorStartTime = millis(); // Record the motor start time
            motorRunning = true; // Set motorRunning flag to true
        }

        // Turn off the motor after 1 second using non-blocking code
        if (motorRunning && millis() - motorStartTime > 1000) {
            digitalWrite(vibrationMotorPin, LOW);  // Turn off the vibration motor
            motorRunning = false; // Reset motorRunning flag
        }

        // Check for weight thresholds and update LEDs
        digitalWrite(ledPin1, weightKg >= 1.0 ? HIGH : LOW);    // LED for 1 kg
        digitalWrite(ledPin05, weightKg >= 0.5 ? HIGH : LOW);   // LED for 0.5 kg
        analogWrite(ledPin0, 100); // LED for 0 kg (should always be on)

    } else {
        Serial.println("Scale not ready.");
    }

    // Update every half a second
    delay(500);
}

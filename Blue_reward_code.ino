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
const float maxThresholdKg = 1.0; // Set threshold for maximum weight

// Timing variables
unsigned long noWeightStartTime = 0;  // Time tracking for no weight
const unsigned long noWeightDelay = 60000; // 10 seconds delay for LED to turn on
bool noWeightDetected = false; // Flag for weight detection state

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
    analogWrite(ledPin0, 0);
    analogWrite(ledPin05, 0);
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
        if (weightKg >= maxThresholdKg && !motorRunning) {
            // Play a single beep through the amplifier/speaker
            tone(amplifierPin, 1000, 200);  // Send a 1000Hz signal to the amplifier for 200ms

            // Turn on the vibration motor
            digitalWrite(vibrationMotorPin, HIGH);
            motorStartTime = millis(); // Record the motor start time
            motorRunning = true; // Set motorRunning flag to true

            // Reset the no weight detection timer
            noWeightDetected = false; // Weight is detected, so reset flag
        }

        // Turn off the motor after 1 second using non-blocking code
        if (motorRunning && millis() - motorStartTime > 1000) {
            digitalWrite(vibrationMotorPin, LOW);  // Turn off the vibration motor
            motorRunning = false; // Reset motorRunning flag
        }

        // Check for weight thresholds and update LEDs
        if (weightKg >= maxThresholdKg) {
            // Weight is above 1 kg, turn on the 1 kg LED and turn off the blue LED
            digitalWrite(ledPin1, HIGH);    // LED for 1 kg
            analogWrite(ledPin05, 0); // Turn off the blue LED
            noWeightDetected = false; // Reset no weight detected flag
        } else {
            // Weight is below 1 kg
            digitalWrite(ledPin1, LOW); // Turn off the 1 kg LED
            // Check if the weight exceeds 0.5 kg
            if (weightKg >= thresholdKg) {
                analogWrite(ledPin05, 0); // Turn off the blue LED
            } else {
                // If weight is below the threshold
                if (!noWeightDetected) {
                    noWeightStartTime = millis(); // Record the time when weight is no longer detected
                    noWeightDetected = true;       // Set the flag to indicate no weight detected
                }

                // Check if 10 seconds have passed since the last weight was detected
                if (millis() - noWeightStartTime >= noWeightDelay) {
                    analogWrite(ledPin05, 100); // Turn on the blue LED after 10 seconds
                }
            }
        }

        // Keep the green LED on for 0 kg threshold
        analogWrite(ledPin0, 100); // Set brightness for the 0 kg LED

    } else {
        Serial.println("Scale not ready.");
        analogWrite(ledPin05, 0); // Turn off the blue LED if the scale is not ready
    }

    // Update every half a second
    delay(500);
}

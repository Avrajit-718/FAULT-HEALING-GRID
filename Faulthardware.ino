// Include necessary libraries
#include <SPI.h>      // For LoRa
#include <LoRa.h>     // If using LoRa SX1278 module
#include <SoftwareSerial.h> // If using Bluetooth HC-05

// Sensor pins
#define VOLTAGE_PIN A0
#define CURRENT_PIN A1

// Fault thresholds
#define VOLTAGE_THRESHOLD 210     // Example threshold value (modify as needed)
#define CURRENT_THRESHOLD 2       // Example threshold value (Amp)

// Communication select
#define USE_LORA      // Comment for Bluetooth usage

#ifdef USE_LORA
// LoRa pins (modify as per connection)
#define SS_PIN 10
#define RST_PIN 9
#define DIO0_PIN 2
#else
SoftwareSerial BTSerial(2, 3); // RX, TX (HC-05)
#endif

void setup() {
  Serial.begin(9600);

#ifdef USE_LORA
  // Initialize LoRa
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  if (!LoRa.begin(433E6)) { // Use 433E6, 868E6, or as per your region
    Serial.println("LoRa init failed!");
    while (1);
  }
  Serial.println("LoRa init OK");
#else
  // Bluetooth init
  BTSerial.begin(9600);
  Serial.println("Bluetooth init OK");
#endif
}

void loop() {
  // Read sensor data
  int voltage_value = analogRead(VOLTAGE_PIN);
  int current_value = analogRead(CURRENT_PIN);

  // Simple conversion (update with your sensor specs)
  float voltage = voltage_value * (5.0 / 1023.0) * 100; // Example mapping
  float current = current_value * (5.0 / 1023.0);       // Example mapping

  String message = "";

  // Fault detection logic
  if (voltage < VOLTAGE_THRESHOLD) {
    message = "Voltage Fault: " + String(voltage) + "V";
  } else if (current > CURRENT_THRESHOLD) {
    message = "Overcurrent Fault: " + String(current) + "A";
  } else {
    message = "Grid OK";
  }

  // Transmit detected problem
#ifdef USE_LORA
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
#else
  BTSerial.println(message);
#endif

  // Local debug
  Serial.println(message);

  delay(1000); // 1 second delay
}

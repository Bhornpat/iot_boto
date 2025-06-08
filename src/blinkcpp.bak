#include <Arduino.h>
#define LED_PIN 23  // Use GPIO23 (you can change to any OUTPUT-safe GPIO)

void setup() {
  // Initialize LED pin as OUTPUT
  pinMode(LED_PIN, OUTPUT);

  // Optional: Start serial monitor for debug
  Serial.begin(115200);
  Serial.println("LED Blink Program Started");
}

void loop() {
  // Turn LED ON
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(500); // Wait 500 ms

  // Turn LED OFF
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(500); // Wait 500 ms
}
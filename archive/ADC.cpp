#include <Arduino.h>
#define POT_PIN 36  // GPIO36 = ADC1_0
#define LED_PIN 23  // Optional: LED output for brightness feedback

void setup() {
  Serial.begin(115200);

  pinMode(POT_PIN, INPUT);     // Not required, but makes code clear
  pinMode(LED_PIN, OUTPUT);    // LED to show brightness (optional)

  Serial.println("ADC Potentiometer Reader Started");
}

void loop() {
  int adcValue = analogRead(POT_PIN);  // 0–4095 (12-bit ADC)

  // Print raw value
  Serial.print("ADC Value: ");
  Serial.println(adcValue);

  // Optional: map to 0–255 PWM range and set LED brightness
  int brightness = map(adcValue, 0, 4095, 0, 255);
  analogWrite(LED_PIN, brightness);  // NOTE: Arduino-style analogWrite is limited

  delay(200);  // Update every 200ms
}
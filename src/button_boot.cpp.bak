#include <Arduino.h>
#define LED_PIN 23   // Output pin for LED
#define BUTTON_PIN 0    // Input pin for push button (BOOT button or external)

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Use internal pull-up resistor

  Serial.println("Button Control LED Initialized");
}

void loop() {
  // Active-low: Pressed = LOW, Released = HIGH
  bool isPressed = digitalRead(BUTTON_PIN) == LOW;

  if (isPressed) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Button Pressed - LED ON");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Button Released - LED OFF");
  }

  delay(100);  // Short delay to avoid bouncing issues
}

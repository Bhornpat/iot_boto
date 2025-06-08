#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <ESP32Servo.h> // ESP32 Servo library

// --- Pin Definitions ---
#define LED_PIN 23    // GPIO pin for the LED
#define DHTPIN 4      // Digital pin connected to the DHT11 sensor (e.g., GPIO 4)
#define SERVO_PIN 13  // GPIO pin for the servo motor (e.g., GPIO 13)

// --- DHT Sensor Type ---
#define DHTTYPE DHT11 // DHT 11 sensor

// --- WiFi Credentials ---
const char* ssid = "PLS MERCY Me"; // REPLACE with your WiFi SSID
const char* password = "11111111"; // REPLACE with your WiFi password

// --- WebServer Object ---
WebServer server(80);

// --- Sensor and Servo Objects ---
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor
Servo myServo;            // Create a servo object

// --- Servo Control Variables for Speed ---
int currentServoAngle = 90; // Start at center, will be updated in loop
int targetServoAngle = 90;  // Default target
int servoMoveDelayMs = 15;  // Delay in milliseconds between each 1-degree step (smaller = faster)
unsigned long lastServoMoveTime = 0; // To track time for non-blocking delay

// --- Web Server Handlers ---

// Root handler
void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS header
  server.send(200, "text/plain", "ESP32 API: Use /on, /off, /dht, /servo?angle=0-180&speed=5-50");
}

// LED ON handler
void handleOn() {
  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS header
  digitalWrite(LED_PIN, HIGH);
  server.send(200, "text/plain", "LED ON");
  Serial.println("LED turned ON");
}

// LED OFF handler
void handleOff() {
  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS header
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "LED OFF");
  Serial.println("LED turned OFF");
}

// DHT Data handler
void handleDhtData() {
  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS header

  // Read temperature and humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();    // Celsius
  float f = dht.readTemperature(true); // Fahrenheit

  // Check if any reads failed and exit early (to avoid NaN values)
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    server.send(500, "text/plain", "Failed to read from DHT sensor!");
    return;
  }

  // Construct the response string
  String response = "Humidity: " + String(h) + "%\n";
  response += "Temperature: " + String(t) + " *C\n";
  response += "Temperature: " + String(f) + " *F";

  server.send(200, "text/plain", response);
  Serial.println("DHT Data Sent:\n" + response);
}

// Servo Control handler
void handleServo() {
  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS header

  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    targetServoAngle = constrain(angle, 0, 180); // Set the target angle

    // Check for optional speed parameter
    if (server.hasArg("speed")) {
        int speed = server.arg("speed").toInt();
        // Map speed parameter (e.g., 1-100) to delayMs (e.g., 50-5ms)
        // Adjust this mapping based on desired speed range
        // Lower speed value (e.g., 1) -> higher delay (slower)
        // Higher speed value (e.g., 100) -> lower delay (faster)
        servoMoveDelayMs = map(speed, 1, 100, 50, 5); // Example mapping: speed 1 is 50ms delay, speed 100 is 5ms delay
        servoMoveDelayMs = constrain(servoMoveDelayMs, 5, 50); // Ensure delay is within reasonable bounds
    } else {
        servoMoveDelayMs = 15; // Default speed if not specified
    }

    String response = "Servo target set to " + String(targetServoAngle) + " degrees with speed delay " + String(servoMoveDelayMs) + "ms";
    server.send(200, "text/plain", response);
    Serial.println(response);
  } else {
    server.send(400, "text/plain", "Bad Request: Missing 'angle' parameter. Use /servo?angle=0-180&speed=1-100");
    Serial.println("Servo command: Missing angle parameter");
  }
}

// --- Function to update servo position gradually ---
void updateServoPosition() {
  // Only move if current angle is not the target angle
  if (currentServoAngle != targetServoAngle) {
    // Check if enough time has passed since the last step
    if (millis() - lastServoMoveTime >= servoMoveDelayMs) {
      if (currentServoAngle < targetServoAngle) {
        currentServoAngle++; // Increment towards target
      } else if (currentServoAngle > targetServoAngle) {
        currentServoAngle--; // Decrement towards target
      }
      myServo.write(currentServoAngle); // Write the new angle
      lastServoMoveTime = millis();    // Update last move time
      Serial.print("Moving servo to: ");
      Serial.println(currentServoAngle);
    }
  }
}

// --- Setup Function ---
void setup() {
  Serial.begin(115200); // Initialize Serial communication
  delay(10); // Small delay for serial to initialize

  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
  digitalWrite(LED_PIN, LOW); // Ensure LED is off initially

  dht.begin(); // Initialize DHT sensor

  // Attach the servo to the specified pin, letting the library use its defaults
  myServo.attach(SERVO_PIN);
  myServo.write(currentServoAngle); // Initialize servo to currentServoAngle
  Serial.println("Servo initialized to 90 degrees.");

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure Web Server Routes
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/dht", handleDhtData);
  server.on("/servo", handleServo);

  server.begin(); // Start the web server

  Serial.println("Web Server running...");
}

// --- Main Loop Function ---
void loop() {
  server.handleClient(); // Handle incoming HTTP requests (non-blocking)
  updateServoPosition(); // Update servo position gradually (non-blocking)
}
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <ESP32Servo.h> // Include the ESP32 Servo library

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

// --- Web Server Handlers ---

// Root handler
void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS header
  server.send(200, "text/plain", "ESP32 API: Use /on, /off, /dht, /servo?angle=0-180");
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
    // Constrain the angle to a valid range (0-180 degrees)
    angle = constrain(angle, 0, 180);
    myServo.write(angle);
    String response = "Servo set to " + String(angle) + " degrees";
    server.send(200, "text/plain", response);
    Serial.println(response);
  } else {
    server.send(400, "text/plain", "Bad Request: Missing 'angle' parameter. Use /servo?angle=0-180");
    Serial.println("Servo command: Missing angle parameter");
  }
}

// --- Setup Function ---
void setup() {
  Serial.begin(115200); // Initialize Serial communication

  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
  digitalWrite(LED_PIN, LOW); // Ensure LED is off initially

  dht.begin(); // Initialize DHT sensor

  // Attach the servo to the specified pin
  // SG90 typically uses 500us for 0 deg and 2500us for 180 deg pulse width
  // You might need to fine-tune these values for your specific servo.
  myServo.attach(SERVO_PIN, 500, 2500);
  myServo.write(90); // Set initial servo position to 90 degrees (center)
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
  server.handleClient(); // Handle incoming HTTP requests
}
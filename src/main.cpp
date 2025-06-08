#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h> // Include the DHT sensor library

#define LED_PIN 23
#define DHTPIN 4    // Digital pin connected to the DHT11 sensor (e.g., GPIO 4)
#define DHTTYPE DHT11 // DHT 11

const char* ssid = "PLS MERCY Me";
const char* password = "11111111";

WebServer server(80);
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

void handleRoot() {
  server.send(200, "text/plain", "ESP32 API: Use /on, /off, or /dht");
}

void handleOn() {
  digitalWrite(LED_PIN, HIGH);
  server.send(200, "text/plain", "LED ON");
}

void handleOff() {
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "LED OFF");
}

void handleDhtData() {
  // Read temperature and humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius
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
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin(); // Initialize DHT sensor

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/dht", handleDhtData); // New handler for DHT data
  server.begin();

  Serial.println("Server running...");
}

void loop() {
  server.handleClient();
}
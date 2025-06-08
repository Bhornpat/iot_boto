#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#define LED_PIN 23

const char* ssid = "PLS MERCY Me";
const char* password = "11111111";

WebServer server(80);
bool ledState = false;

void handleRoot() {
  String html = "<html><head><title>ESP32 LED</title></head><body>";
  html += "<h1>ESP32 LED Control</h1>";
  html += "<p>LED is currently " + String(ledState ? "ON" : "OFF") + "</p>";
  html += "<form action=\"/toggle\" method=\"get\">";
  html += "<button type=\"submit\">Toggle LED</button></form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleToggle() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  handleRoot();  // Redirect back to main page
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected! IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();

  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

/*
 * 🌐 Firebase IoT Environment Monitor (Task 5)
 * 
 * Hardware:
 * - ESP32
 * - DHT11 (GPIO 4)
 * - LDR (GPIO 34)
 * - 2-Channel Relay (GPIO 5)
 *
 * IMPORTANT: Use placeholders for credentials.
 */

#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHT.h"

// -------------------------
// Credentials (Placeholders)
// -------------------------
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define FIREBASE_HOST "YOUR_FIREBASE_PROJECT_ID.firebaseio.com"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
// For Firebase Authentication:
#define USER_EMAIL "ESP32_ACCOUNT_EMAIL"
#define USER_PASSWORD "ESP32_ACCOUNT_PASSWORD"

// -------------------------
// Pins & Settings
// -------------------------
#define DHTPIN 4
#define DHTTYPE DHT11
#define LDR_PIN 34
#define RELAY_PIN 5

DHT dht(DHTPIN, DHTTYPE);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// -------------------------
// Timing Intervals
// -------------------------
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL_MS = 3000;

unsigned long lastLogTime = 0;
const unsigned long LOG_INTERVAL_MS = 15000;

// -------------------------
// State Variables
// -------------------------
String currentMode = "AUTO"; 
bool bulbState = false;
bool autoBulbState = false; // Hysteresis state tracker

void setup() {
  Serial.begin(115200);
  
  pinMode(LDR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Default OFF
  
  dht.begin();
  
  // Connect Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Firebase Setup
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  // Alternative Auth (Email/Password) if using newer Firebase Client
  /*
  config.api_key = "YOUR_WEB_API_KEY";
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  */

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // Set initial status
  Firebase.setString(fbdo, "/status/online", "true");
  
  Serial.println("Firebase Connected & Ready.");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check Control Mode from Firebase continuously
  if (Firebase.getString(fbdo, "/control/mode")) {
    currentMode = fbdo.stringData();
  }
  if (Firebase.getString(fbdo, "/control/bulb")) {
    String bulbCmd = fbdo.stringData();
    if (currentMode == "MANUAL") {
      bulbState = (bulbCmd == "ON");
      digitalWrite(RELAY_PIN, bulbState ? HIGH : LOW);
    }
  }

  // 1. Read Sensors (Every 3s)
  if (currentMillis - lastReadTime >= READ_INTERVAL_MS) {
    lastReadTime = currentMillis;
    
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    int light = analogRead(LDR_PIN);
    
    if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    
    Serial.printf("Temp: %.1f°C | Hum: %.1f%% | LDR: %d | Mode: %s\n", t, h, light, currentMode.c_str());
    
    // Update live sensors
    Firebase.setFloat(fbdo, "/sensors/temperature", t);
    Firebase.setFloat(fbdo, "/sensors/humidity", h);
    Firebase.setInt(fbdo, "/sensors/light", light);
    
    // 2. Automation Logic
    if (currentMode == "AUTO") {
      // Trigger ON: Temperature >= 30°C OR Humidity >= 70%
      if (!autoBulbState && (t >= 30.0 || h >= 70.0)) {
        autoBulbState = true;
        bulbState = true;
        digitalWrite(RELAY_PIN, HIGH);
        Firebase.setString(fbdo, "/control/bulb", "ON");
        Serial.println("AUTO Trigger: ON");
      }
      // Trigger OFF: Temperature <= 26°C AND Humidity < 70%
      else if (autoBulbState && (t <= 26.0 && h < 70.0)) {
        autoBulbState = false;
        bulbState = false;
        digitalWrite(RELAY_PIN, LOW);
        Firebase.setString(fbdo, "/control/bulb", "OFF");
        Serial.println("AUTO Trigger: OFF");
      }
    }
  }
  
  // 3. Historical Logging (Every 15s)
  if (currentMillis - lastLogTime >= LOG_INTERVAL_MS) {
    lastLogTime = currentMillis;
    
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    int light = analogRead(LDR_PIN);
    
    if (!isnan(t) && !isnan(h)) {
      // Construct JSON
      FirebaseJson json;
      json.set("temperature", t);
      json.set("humidity", h);
      json.set("light", light);
      json.set("mode", currentMode);
      json.set("bulb", bulbState ? "ON" : "OFF");
      json.set("timestamp", ".sv/timestamp"); // Server timestamp
      
      if (Firebase.pushJSON(fbdo, "/logs", json)) {
        Serial.println("Log pushed successfully.");
      } else {
        Serial.println("Failed to push log: " + fbdo.errorReason());
      }
    }
  }
}

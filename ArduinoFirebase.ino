#include <WiFiNINA.h>
#include <FirebaseESP32.h>

// Firebase credentials
#define FIREBASE_HOST "traffic-light-s223130154-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "........................" // Have changed it for security

// WiFi credentials
#define WIFI_SSID "SHIVJI"
#define WIFI_PASSWORD "Dreams11"

// LED pins
const int redLED = 2;
const int greenLED = 3;
const int blueLED = 4;

// Firebase objects
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// WiFi reconnection parameters
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // 5 seconds

void setup() {
  Serial.begin(9600);  // Set baud rate to 9600

  connectToWiFi();
  initializeFirebase();

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  // Listen for changes in Firebase
  if (!Firebase.beginStream(firebaseData, "/LEDcolor")) {
    Serial.println("Failed to begin stream");
    Serial.println("REASON: " + firebaseData.errorReason());
  } else {
    Serial.println("Stream started...");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }

  if (!Firebase.readStream(firebaseData)) {
    Serial.println("Stream read failed: " + firebaseData.errorReason());
  } else if (firebaseData.streamAvailable()) {
    Serial.println("Stream data available...");
    handleStreamData(firebaseData);
  }
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void reconnectWiFi() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastReconnectAttempt >= reconnectInterval) {
    lastReconnectAttempt = currentMillis;
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconnected to WiFi");
    } else {
      Serial.println("Failed to reconnect to WiFi");
    }
  }
}

void initializeFirebase() {
  // Initialize Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void handleStreamData(FirebaseData &data) {
  String path = data.dataPath();
  bool state = data.boolData();

  Serial.print("Path: ");
  Serial.print(path);
  Serial.print(", Value: ");
  Serial.println(state);

  if (path == "/Red") {
    digitalWrite(redLED, state ? HIGH : LOW);
  } else if (path == "/Green") {
    digitalWrite(greenLED, state ? HIGH : LOW);
  } else if (path == "/Blue") {
    digitalWrite(blueLED, state ? HIGH : LOW);
  }
}

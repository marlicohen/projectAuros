/* Kurt Le, Marli Cohen, Gabriela Clemente
 * CS 435 Embedded Systems
 * <12/10/2025>
 * Final Project
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>
#include <Adafruit_GPS.h>
#include "Adafruit_seesaw.h"

// --- WiFi credentials ---
const char* ssid     = "Middlebury-IoT";
const char* password = "indish80recur";

// --- MQTT Broker info ---
const char* mqtt_server = "test.mosquitto.org";
const int   mqtt_port   = 1883;
const char* mqtt_topic_coords    = "feather/test"; // Test chanel
const char* mqtt_topic_location  = "feather/location"; // Long & Lat Chanel
const char* mqtt_topic_sos       = "feather/sos"; // Publish both SOS signal and Long & Lat at the current time
const char* mqtt_topic_buzzerN = "feather/buzzerNotification"; // Buzzer notificaiton

// --- GPS Serial pins ---
int rxPin = 16;
int txPin = 17;
HardwareSerial GPSSerial(1);
Adafruit_GPS GPS(&GPSSerial);

// --- Joy FeatherWing (seesaw) ---
Adafruit_seesaw ss;
#define BUTTON_RIGHT 6
#define BUTTON_DOWN  7
#define BUTTON_LEFT  9
#define BUTTON_UP    10
#define BUTTON_SEL   14
#define BUZZER_PIN   27

uint32_t button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
                       (1 << BUTTON_LEFT) | (1 << BUTTON_UP) | (1 << BUTTON_SEL);

#if defined(ESP8266)
  #define IRQ_PIN   2
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
  #define IRQ_PIN   14
#else
  #define IRQ_PIN   5
#endif

// --- MQTT client ---
WiFiClient espClient;
PubSubClient client(espClient);


unsigned long lastLocationPublish = 0;
const unsigned long locationInterval = 2000; // 2 seconds

void publishSOS(float lat, float lon) {
  char sosMsg[128];
  snprintf(sosMsg, sizeof(sosMsg),
           "{\"sos\": true, \"lat\": %.6f, \"lon\": %.6f}",
           lat, lon);

  Serial.print("Publishing SOS to topic ");
  Serial.print(mqtt_topic_sos);
  Serial.print(": ");
  Serial.println(sosMsg);

  client.publish(mqtt_topic_sos, sosMsg);
}

void publishjustLocation(float lat, float lon) {
  char losMsg[128];
  snprintf(losMsg, sizeof(losMsg),
         "lat: %.6f, lon: %.6f",
         lat, lon);
  Serial.print("Publishing SOS to topic ");
  Serial.print(mqtt_topic_location);
  Serial.print(": ");
  Serial.println(losMsg);

  client.publish(mqtt_topic_location, losMsg);
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("esp32_gps_client")) {
      client.subscribe(mqtt_topic_buzzerN);
      Serial.println("connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" — retrying in 2s");
      delay(2000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Convert payload to a string
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Buzz on ANY message received on this topic
  Serial.println("Triggering buzzer from MQTT signal");
  digitalWrite(BUZZER_PIN, HIGH);   // buzzer ON
  delay(3000);                      // buzz for 1 second
  digitalWrite(BUZZER_PIN, LOW);    // buzzer OFF
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 + GPS + MQTT + JoyWing starting...");

  // GPS setup
  GPSSerial.begin(9600, SERIAL_8N1, rxPin, txPin);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

  // WiFi + MQTT
  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Joy FeatherWing setup
  if(!ss.begin(0x49)){
    Serial.println("ERROR! seesaw not found");
    while(1) delay(1);
  } else {
    Serial.println("seesaw started");
    Serial.print("version: ");
    Serial.println(ss.getVersion(), HEX);
  }
  ss.pinModeBulk(button_mask, INPUT_PULLUP);
  ss.setGPIOInterrupts(button_mask, 1);
  pinMode(IRQ_PIN, INPUT);

  // --- Buzzer setup ---
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // buzzer off initially
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  GPS.read();
  if (GPS.newNMEAreceived()) {
    char *nmea = GPS.lastNMEA();
    if (!GPS.parse(nmea)) {
      return;
    }
  }

  // --- SOS button check (Button RIGHT) ---
  uint32_t buttons = ss.digitalReadBulk(button_mask);
  if (!(buttons & (1 << BUTTON_RIGHT))) {
    Serial.println("Button RIGHT pressed -> Sending SOS");
    if (GPS.fix) {
      publishSOS(GPS.latitudeDegrees, GPS.longitudeDegrees);
    } else {
      publishSOS(0.0, 0.0); // fallback if no GPS fix yet
    }
    delay(500); // debounce
  }

  // --- Timed location publishing every 2s ---
  unsigned long now = millis();
  if (now - lastLocationPublish >= locationInterval) {
    lastLocationPublish = now;
    if (GPS.fix) {
      publishjustLocation(GPS.latitudeDegrees, GPS.longitudeDegrees);
    } else {
      publishjustLocation(0.0, 0.0); // fallback if no GPS fix yet
    }
  }

  delay(50);
}
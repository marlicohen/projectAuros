// Final Project
// Kurt Le, Marli Cohen, Gabi Clemente
// Date: 12/10/2025


#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <PubSubClient.h>


//--------------------Setting Up Display----------------------------
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire); // what does this do?
// Setting up buttons
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14



//--------------------Setting Up Wifi & Broker-------------------------------
// Wifi
const char* ssid = "Middlebury-IoT";  
const char* password = "trials40outadded";  //YOUR MIDD WIFI NETWORK!!!!!

// MQTT Broker
const char* mqtt_server = "test.mosquitto.org"; //Just a name I have right now We can name it something else
const int mqtt_port = 1883;   // TLS port

// Wifi and PubSub clients
WiFiClient espClient;
PubSubClient client(espClient);

// Channels
const char* pubChannel = "feather/buzzerNotification";
const char* sosChannel = "feather/sos";
const char* locChannel = "feather/location";

// setting up Wifi
void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

//-----------------Setting global variables--------------------------------------
bool sos = false;
bool loc = false;
char coords[50]; 
uint8_t signifier = 0;
char msg[128];  

//---------------Pub Sub Functions------------------------------------------------
//reconnecting
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("espFeatherClient")) {
      //client.publish(pubChannel, "connected");
      client.subscribe(sosChannel);
      client.subscribe(locChannel);
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}
void displayInfo() {
  Serial.println("New Display");
  Serial.print("SOS: ");
  Serial.println(sos);
  display.clearDisplay();
  display.setCursor(0,0);
  if (sos == true) {
      display.setTextSize(2);
      display.println("SOS!");
      display.setTextSize(1);
      display.println();
  } else {
    display.setTextSize(1);
    display.println("Status: All Good");
    display.println();
  }
  display.println("Last Location:");
  if (loc == true) {
    display.print("  ");
    for (int i = 0; i < 12; i++) {
      display.print(coords[i]);
    }
    display.println(coords[12]);
    display.print("  ");
    for (int i = 15; i < 30; i++) {
      display.print(coords[i]);
    }
  } else {
    display.println("Not Found");
  }
  display.display();
}

//callback function
void callback(char *topic, byte *payload, unsigned int length) {

  //Check if we received an sos
  if (strcmp(topic, sosChannel) == 0) {
    Serial.println("sos channel updated");
    //updating SOS variable
    sos = true;
  }

  //Check if we received a location
  if (strcmp(topic, locChannel) == 0) {
    Serial.println("location channel updated");
    // Updates location bool
    loc = true;
    // Updates location message
    for (int i = 0; i < length; i++) {
      char character = (char)payload[i];
      coords[i] = character;
    }
    coords[length] = '\0';
  }
  Serial.print("SOS value on callback: ");
  Serial.println(sos);
  displayInfo();
}


void setup() {
  Serial.begin(115200);

  Serial.println("128x64 OLED FeatherWing test");
  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true); // Address 0x3C default

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setRotation(1); //what does this do?
  Serial.println("Button test");

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println("Status: All Good!");
  display.println();
  display.println("Last Location:");
  display.println("Not Found");
  display.display();

  //Configuring Wifi & Broker
  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);   
}

void loop() {
  //Buttons
  if (!digitalRead(BUTTON_A) || !digitalRead(BUTTON_B) || !digitalRead(BUTTON_C)) {
    // Publishing to channel
    signifier= signifier + 1; 
    snprintf(msg, 50, "buzz #%u", signifier);
    client.publish(pubChannel, msg);

    //Turning off the SOS signal
    sos = false;
    displayInfo();
    delay(500);
  }

  //Broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
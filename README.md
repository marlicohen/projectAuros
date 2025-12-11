# Project Auros
Kurt Le, Marli Cohen, Gabriela Clemente 

##Introduction
Project Auros is a wearable emergency alert system designed to be embedded within a piece of jewelry. The goal of the system is to enable users to send an SOS message or transmit their live location to a designated receiver through WiFi. The receiver displays the transmitted information and can send a confirmation signal back to the wearable device. This confirmation is communicated to the user through a vibration, providing reassurance that their message has been received. 

This project was developed as part of an Embedded Systems course and reflects our learning goals in integrating microcontrollers, wireless communication, sensors, and user-centered hardware design. Our initial motivation stemmed from the desire to design a safety tool primarily aimed at vulnerable populations, including women walking alone. Over time, the project evolved into a general emergency communication device suitable for anyone who needs to rapidly call for help without needing to reach for a phone.

Throughout the design phase, we consulted documentations as well as tutorials from Adafruit(ESP32 Feather, GPS FeatherWing, Joy FeatherWing, and OLED FeatherWing), Mosquitto MQTT documentation, and user centered accessibility frameworks such as the Web Accessibility Initiative and Usability.gov  guidelines. 


##Methods 
###System Overview:
Project Auros consists of two microcontroller based devices, the transmitter and the receiver. 
###Transmitter:
The transmitter contains one button for user interaction, which is used to send out the SOS signal. This signal is then sent to the receiver via the MQTT channel. The transmitter also has a GPS system which obtains the user’s location and sends it to the receiver every two seconds through MQTT. Additionally, the transmitter is equipped with a vibrating motor, which will be used to provide confirmation feedback. Ideally, the transmitter would be wearable–embedded within jewelry casing or used as a small keychain.

###Receiver:
The receiver will be continuously polling the Mosquitto channel for new messages and updates from the transmitter. In addition, The receiver device is equipped with buttons to send out a “read” signal to the transmitter as confirmation.  Once a message has been received, the receiver will display the message on the display board (OLED FeatherWing).

Both of the devices include an ESP32 microcontroller and are intended to operate off battery power for portability and independence. 

Hardware and Peripherals: 
(2) ESP32 Featherboard:
The ESP32 Featherboard was chosen due to its low power consumption, built-in WiFI support, as well as its compatibility with the other FeatherWing peripherals. 

(1) GPS FeatherWing:
The GPS FeatherWing is compatible with the ESP32 and enables periodic real-time location tracking. In our design, it refreshes every 2 seconds.

(1) Joy FeatherWing:
The Joy FeatherWing is equipped with 4 buttons, one of  which is used to send the SOS message via the Mosquitto channel

(1) OLED FeatherWing (Receiver):
The OLED FeatherWing offers a compact readable display for the incoming messages received via the Mosquitto channel. Additionally, the display board is compatible with the ESP32 microcontroller and is equipped with buttons which can be pressed to send the confirmation to the transmitter.

(1) Vibration Motors (Mini Motor):
Provides feedback to the user that their SOS message has been read, which is a crucial aspect both for usability and accessibility. 

Battery System:
(2) Lithium ion battery for the ESP32’s
(1) CR1220 coin battery for the GPS
Together these two battery options allow for approximately 90 minutes of operation.


Software and Libraries
To execute the desired functionality of the designed product, we used the Arduino Framework supported by the following libraries: Adafruit_GFX, Adafruit_SH110X, WiFi.h, Adafruit_GPS, PubSubClient, HardwareSerial, Adafruit_seesaw. 
Below we disclose the central logic for usage of the aforementioned libraries:
	Adafruit_GFX: contains Adafruit graphics to be displayed on the OLED Display
Adafruit_SH110X: controls OLED FeatherWing display
	Wifi.h: connects the device to wifi
Adafruit_GPS.h: handles the communication with the GPS modules and automatically parses data like latitude and longitude, speed, and time
PubSubClient.h: controls interactions with the MQTT server including connecting, publishing and subscribing
HardwareSerial.h: uses the ESP32’s built-in hardware UART port to communicate with external devices (like a GPS) while keeping the default Serial for debugging.
Adafruit_seesaw.h: acts as a flexible I2C interface to provide easy connection to the Joy FeatherWing
	
Transmitter Setup:
The transmitter setup configures the MQTT, GPS, JoyWing button A, and the vibrating motor pin. It also uses PubSubClient.h to set the callback function for the MQTT topic: feather/buzzerNotification. The callback function turns on the vibrating motor pin when a message is received on feather/buzzerNotification.

Transmitter loop:
The loop function first runs a function called “reconnectMQTT()”, which uses PubSubClient.h and Wifi.h to reconnect the device to the MQTT channel and re-subscribe if connection is lost. After “reconnectMQTT()”, the code uses Adafruit_GPS.h to acquire the GPS coordinates of the device and returns 0.000000 for the latitude and longitude if GPS does have a signal. Next, the code uses Adafruit_seesaw.h to read the value of Button A on the JoyWing. If the button is pressed, “publishSOS()” is called, which uses PubSubClient.h to publish a message to topic: feather/sos. Then, every two seconds, if the GPS coordinates are available,  “publishLocation()” is called. This function uses PubSubClient.h to publish the GPS coordinates to topic: feather/location. 

Receiver Setup:
The receiver setup configures MQTT, the OLED FeatherWing display screen, and the OLED FeatherWing buttons. It prints an initial message to the display using Adafruit_GFX and Adafruit_SH110X libraries. It also uses PubSubClient.h to set the callback function for the MQTT feather/sos and feather/location channels. The callback function sets the “sos” boolean to true if a message is received from the feather/sos channel. It sets the “loc” boolean to true if a message is received from the feather/location channel and stores the message in an array. Then the callback calls “displayInfo()”, which prints an sos message and location message based off of the booleans. If sos is true, “SOS!” is printed. If false, “Status: All Good” is displayed. If location is false, the code prints “Location: Not found”. If location is true, the coordinates of the GPS are displayed. 

Receiver loop:
The receiver loop function first checks if any of the buttons are pressed. If so, it uses PubSubClient.h to publish a message to feather/buzzerNotification, sets the sos message to false, and calls “displayInfo()”. It then checks if the client is disconnected from MQTT and reconnects it if not.

User Interface Design
Transmitter User Interface:
The transmitter is equipped with one button which can be pressed by the user to trigger the SOS signal. Additionally, the device is equipped with the vibrating motor which vibrates to confirm that someone has received their sos message. There is no visual interface so as to maintain discretion.

Receiver User Interface:
The receiver contains three buttons. Any of these buttons can be pushed to send the “received SOS” message. Additionally, it is equipped with the OLED used to display the SOS message and latest GPS coordinates.

The simple interaction model of the design ensures minimal cognitive load at the moment of the emergency. 

Bill of Materials 
Items
Unit Price (in USD)
Link
Supplier
Quantity
Total Cost (in USD)
FeatherWing OLED
14.95
https://www.adafruit.com/product/4650 
Adafruit
1
14.95
GPS FeatherWing
24.95
https://www.adafruit.com/product/3133 
Adafruit 
1
24.95
Joy FeatherWing
9.95
https://www.adafruit.com/product/3632 
Adafruit
1
9.95
Vibrating Disc x2
1.95
https://www.adafruit.com/product/1201 
Adafruit
2
3.90
Lithium Ion Polymer Battery (for Esp 32)
9.95 
https://www.adafruit.com/product/258 
Adafruit
2
19.90 
CR1220 Coin Battery (for GPS)
.72
digikey 
DigiKey
10
7.16
Processors x2
0
Already purchased
Adafruit
2
N/A


 Subtotal(shipping not included): 80.81 dollars
UPS ground estimate: 12.29
Total: $93.10


Results
The final prototype successfully demonstrates functionality: the transmitter sends an SOS message to the Mosquito channel on a button-push, and the location is also sent to the channel every 2 seconds (when the GPS has signal). The receiver polls and displays new messages in real time, and pressing the receiver buttons successfully makes the transmitter vibrate for 3 seconds. 

Challenges & Limitations:
When the GPS is indoors or obstructed, it may fail to acquire the location, leading to delayed or inaccurate location reports. This behavior is consistent with known limitations of low-power GPS modules. If connection is lost, the device will not automatically reconnect–the user will have to manually refresh the GPS device to re-establish connection. 

Both devices must be connected to Wifi in order to publish and subscribe to MQTT. Therefore, if one of the devices is not connected to Wifi, it will not function properly.

Additionally, the transmitter is too large to be conveniently worn as a necklace/jewelry charm. Future work would use only the necessary parts instead of featherwings in order to scale the device down.

Comparison With Existing Systems:
Commercial personal safety devices typically rely on cellular networks and proprietary apps. Project Auros differs by WiFi and MQTT, lowering cost but also limiting range and reliability. This is acceptable for a prototype but would require expansion for real deployment. 


Accessibility Considerations
The device was evaluated with respect to dexterity, sensory, and usability constraints:
Dexterity:
Regarding dexterity, the small buttons on the jewelry may be challenging for users with limited fine motor control, which in turn may lead to failed or accidental activation. 

Visual Impairments:
The transmitter uses fully non-visual input and output. However, the user with the receiver must have functioning vision in order to read the displayed messages. 

Auditory Impairments:
No sound based input or output are necessary. Therefore, the system is neutral for those with hearing impairments.

Tactile/ Sensory Impairments: 
Users with reduced tactile sensitivity may struggle to feel activation or vibration feedback.

Cognitive:
As a small system, users faced with some form of cognitive challenge may not recall where they placed the device. However, when structured as a keychain, or necklace, users would always have the device handy and ready to be engaged.

Ethical Implications
Project Auros raises several ethical considerations including:
Privacy and Data Security:
The SOS messages and GPS coordinates collected from users are sensitive, and therefore must be protected from external interception.

Reliability and Safety:
As an emergency device, failures in WiFi, battery, or GPS may endanger the user by creating a false expectation that help is on the way. 

Informed Use:
Users must be made aware and understand how location tracking works, what it transmits, as well as its limitations. 

Risk of Misuse:
Similarly to any other tracking system, improper access could expose the user to unnecessary harm.

Hardware Sourcing:
As with any computer system, the environmental sustainability of obtaining hardware and the treatment of workers involved in the process are concerns.







Schedule
Week
Planned Schedule
Actual Progress
Week 6
Final planning and submission of project proposal; list and order the parts needed for project completion.
Submitted project proposal and ordered the hardware needed.
Week 7
Begin designing hardware assembly and researching wiring and integration; Look for code references and understand datasheets.
While waiting for the parts, focus shifted to reviewing documentation and preparation for hardware assembly.
Week 8
Start assembling the hardware focusing on underlying mechanisms and communication between ESP32 FeatherWing boards.
Configured GPS module and connected the GPS FeatherWing to ESP32. Successfully printed current location with reference to google maps.
Week 9
Continue assembling hardware, implement buttons, vibration motor, signal and response logic, as well as begin work on core coding.
Configured and tested OLED display FeatherWing. Successfully connected it to the ESP32 and confirmed proper text output
Week 10 
Final assembly details; Begin drafting final presentation.
Focused on MQTT communication
Week 11
Complete presentation and finalize the prototype.
Continued work on publishing and subscribing to MQTT.  Assembled transmitter device using I2C
Week 12


Ensured button presses on devices resulted in changes to the other device.
Ensured location updates were displayed on the receiver.










Issues and Resolutions 
GPS failure to acquire location:
When faced with the issue of the GPS failing indoors, due to its inability to acquire a location, we decided to test the GPS outdoors, retrying the same parameter, however without any obstructions, and we were successfully able to acquire a location. 

MQTT messages occasionally dropping:
We faced an additional challenge with WiFi instability, which prevented the MQTT network from connecting to our communication devices. The HiveMQ server itself requires a fixed IP address for each device in order to communicate back and forth. However, the MiddleburyIoT network employs a preliminary defense system against cyber threats by periodically changing each device’s IP address at unpredictable intervals. As a result, HiveMQ was not the best option. In contrast, Mosquitto worked better as a lightweight server for IoT devices with very basic requirements.

Battery life:
From the beginning, this prototype faced challenges regarding battery life, the major issue culminated in the battery life of the external batteries we selected for the prototype being too short to sustain the continued use of the GPS tracking activity. To preserve the battery, we used a computer connection to provide power during initial development and testing.

Future Work
Our first improvement concerns are wearability and discreteness. We aim to miniaturize the hardware so as to embed it into a pendant or a keychain. The second improvement concerns battery life. Implementation of a sleep cycle on the GPS module would allow the device to use less power. Next, to ensure user-safety, we would add an encryption factor to the Mosquitto channel and an authentication access on the channel, securing the SOS message and the users’ locations. Given our apprehension regarding the lack of data transmission and the reduction in accuracy of the device, we would like to incorporate in the device a LTE/BLE fallback for instances where there is low or no wifi. Finally, we aim to implement touch sensors and other alternative activation modes to enhance accessibility and make this device more inclusive. 


References
Kirankanukollu. “Making the Obtained Latitude and Longitude into Google Maps URL.” Arduino Forum, 13 Mar. 2017, forum.arduino.cc/t/making-the-obtained-latitude-and-longitude-into-google-maps-url/444369/4. 
Ada, Lady. “Adafruit Ultimate GPS Featherwing.” Adafruit Learning System, learn.adafruit.com/adafruit-ultimate-gps-featherwing?view=all. Accessed 10 Dec. 2025. 
Adafruit. “Adafruit_sh110x/Adafruit_sh1107.Cpp at Master · Adafruit/Adafruit_sh110x.” GitHub, github.com/adafruit/Adafruit_SH110x/blob/master/Adafruit_SH1107.cpp. Accessed 10 Dec. 2025. 

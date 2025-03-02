/* base code credits
  WiFi UDP Send and Receive String
  This sketch waits for a UDP packet on localPort using the WiFi module.
  When a packet is received an Acknowledge packet is sent to the client on port remotePort
  created 30 December 2012
  by dlf (Metodo2 srl)
  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#wi-fi-udp-send-receive-string
 */
 /*
  LHNT Rover HSI + VR EMG team Wifi UDP sending commands
 */
#include <WiFiS3.h>
//#include <string>
int status = WL_IDLE_STATUS;
//#include <WiFiUdp.h>
const char* ssid = "ESP32-LHNT";
const char* password = "longhorns";
const int localUdpPort = 1234;  // Port for receiving
WiFiUDP Udp;
void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, password);
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localUdpPort);
}
// char command_speed = "1 200" // TEMP PLACEHOLDER EXAMPLE forward at speed 200
void send_packet(String command_speed) {
  // command_speed is a string to send to the esp32
  // makes the input into proper format
  Udp.beginPacket("192.168.4.1", 1234);
  Udp.write(command_speed.c_str());
  Udp.endPacket();
  Serial.println("Sent packet with: " + command_speed);
}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
void loop(){
  send_packet("2");
  delay(1000);
}

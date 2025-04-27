/*
  Written by Electronics: Ibrahim & Vic, and HSI: Michael, Sierra, Aashvi, Rishik
  Last Updated 4/18/2025
  LHNT Rover Spring 2025
*/

// This code uses 2 motor drivers, has 7 possible degrees of freedom (forward, backward, left, right, strafe left, strafe right, stop)
// Stops the rover after a given time interval. 
// Details on time interval and speed are given by the report system

// From HSI
#include <WiFi.h>
#include <WiFiUdp.h>
const char* ssid = "ESP32-LHNT";
const char* password = "longhorns";
const int localUdpPort = 4210;  // Port for receiving
WiFiUDP udp;

// Motor A
const int MOTOR_A_PIN_1 = 27;
const int MOTOR_A_PIN_2 = 26;
const int ENABLE_A_PIN = 14;
// Motor B
const int MOTOR_B_PIN_1 = 25;
const int MOTOR_B_PIN_2 = 33;
const int ENABLE_B_PIN = 32;
// Motor C
const int MOTOR_C_PIN_1 = 18;
const int MOTOR_C_PIN_2 = 5;
const int ENABLE_C_PIN = 19;
// Motor D
const int MOTOR_D_PIN_1 = 4;
const int MOTOR_D_PIN_2 = 2;
const int ENABLE_D_PIN = 15;

// Setting motor A PWM properties
const int FREQUENCY_A = 30000;
const int PWM_CHANNEL_A = 0;
const int RESOLUTION_A = 8;
int dutyCycleA = 190; // controls speed
// Setting motor B PWM properties
const int FREQUENCY_B = 30000;
const int PWM_CHANNEL_B = 0; // uses same channel as A. Can be changed if we want finer motor control.
const int RESOLUTION_B = 8;
int dutyCycleB = 190; // controls speed
// Setting motor C PWM properties
const int FREQUENCY_C = 30000;
const int PWM_CHANNEL_C = 0;
const int RESOLUTION_C = 8;
int dutyCycleC = 190; // controls speed
// Setting motor D PWM properties
const int FREQUENCY_D = 30000;
const int PWM_CHANNEL_D = 0; // uses same channel as A. Can be changed if we want finer motor control.
const int RESOLUTION_D = 8;
int dutyCycleD = 190; // controls speed

// WiFi Programmable Global Variables (and their default values)
int timer_delay = 200;    // set with 't' command 
int speed = 190;          // set with 's' command
int debug_level = 1;      // set with 'd' command
bool emergency_stop = false;     // kill with 'e' command
// Other non-action BLE Commands
// send report            // request with 'r' command

// DEVICE IP ADDRESSES <-- Can change non esp address with the register command 'r'
String espAddress = "192.168.4.1";
String pythonAddress = "192.168.4.2";
String unoR4Address = "192.168.4.3";

// Autostop
long time_since_last_command = 0;
bool autostop_enable = true;

void stopLeft() {
  digitalWrite(MOTOR_A_PIN_1, LOW);
  digitalWrite(MOTOR_A_PIN_2, LOW);
  digitalWrite(MOTOR_D_PIN_1, LOW);
  digitalWrite(MOTOR_D_PIN_2, LOW);
}
void stopRight() {
  digitalWrite(MOTOR_B_PIN_1, LOW);
  digitalWrite(MOTOR_B_PIN_2, LOW);
  digitalWrite(MOTOR_C_PIN_1, LOW);
  digitalWrite(MOTOR_C_PIN_2, LOW);
}
void leftForward() {
  digitalWrite(MOTOR_A_PIN_1, HIGH);
  digitalWrite(MOTOR_A_PIN_2, LOW);
  digitalWrite(MOTOR_D_PIN_1, HIGH);
  digitalWrite(MOTOR_D_PIN_2, LOW);
}
void leftBackward() {
  digitalWrite(MOTOR_A_PIN_1, LOW);
  digitalWrite(MOTOR_A_PIN_2, HIGH);
  digitalWrite(MOTOR_D_PIN_1, LOW);
  digitalWrite(MOTOR_D_PIN_2, HIGH);
}
void rightForward() {
  digitalWrite(MOTOR_B_PIN_1, HIGH);
  digitalWrite(MOTOR_B_PIN_2, LOW);
  digitalWrite(MOTOR_C_PIN_1, HIGH);
  digitalWrite(MOTOR_C_PIN_2, LOW);
}
void rightBackward() {
  digitalWrite(MOTOR_B_PIN_1, LOW);
  digitalWrite(MOTOR_B_PIN_2, HIGH);
  digitalWrite(MOTOR_C_PIN_1, LOW);
  digitalWrite(MOTOR_C_PIN_2, HIGH);
}
void forward() {
  leftForward();
  rightForward();
}
void backward() {
  leftBackward();
  rightBackward();
}
void turnLeft() {
  leftBackward();
  rightForward();
}
void turnRight() {
  leftForward();
  rightBackward();
}
void strafeLeft() {
  // A & C forward, B & D backward (or vice versa) to strafe
  digitalWrite(MOTOR_A_PIN_1, HIGH);
  digitalWrite(MOTOR_A_PIN_2, LOW);
  digitalWrite(MOTOR_B_PIN_1, LOW);
  digitalWrite(MOTOR_B_PIN_2, HIGH);
  digitalWrite(MOTOR_C_PIN_1, HIGH);
  digitalWrite(MOTOR_C_PIN_2, LOW);
  digitalWrite(MOTOR_D_PIN_1, LOW);
  digitalWrite(MOTOR_D_PIN_2, HIGH);
}

void strafeRight() {
  digitalWrite(MOTOR_A_PIN_1, LOW);
  digitalWrite(MOTOR_A_PIN_2, HIGH);
  digitalWrite(MOTOR_B_PIN_1, HIGH);
  digitalWrite(MOTOR_B_PIN_2, LOW);
  digitalWrite(MOTOR_C_PIN_1, LOW);
  digitalWrite(MOTOR_C_PIN_2, HIGH);
  digitalWrite(MOTOR_D_PIN_1, HIGH);
  digitalWrite(MOTOR_D_PIN_2, LOW);
}
void stopAll() {
  stopLeft();
  stopRight();
}

// ===================== LOGGING COMMUNICATIONS =====================
void sendToPython(String message) {
  udp.beginPacket(pythonAddress.c_str(), 4210);
  udp.print(message.c_str());
  udp.endPacket();
  Serial.println("Sent packet with: " + message);
}
String getMessageHeader(int type) {
  switch (type) {
    case 0: return "[ESP EMERGENCY]: "; // non-suppressible
    case 1: return "[ESP NOTIF]: ";
    case 2: return "[ESP INFO ]: ";
    case 3: return "[ESP LOG  ]: ";
    default: return "[ESP DEBUG]: ";
  }
}
void log(String message, int type) {
  if (debug_level >= type) {
    sendToPython(getMessageHeader(type) + message);
  }
  Serial.println(message);
}
void log(String message, char type) {
  switch (type) {
    case 'n': return log(message, 1);
    case 'i': return log(message, 2);
    case 'd': return log(message, 4);
    case 'l': // do the default
    default: return log(message, 3);
  }
}
void log(String message) {
  log(message, 3);
}
// ===================== ADMIN ACTIONS =====================
void sendReport() {
  String content = "\n==== LHNT Rover Info ====\n";
  content += "Timer Delay: " + String(timer_delay) + "\n";
  content += "Speed: " + String(speed) + "\n";
  content += "Debug Level: " + String(debug_level) + "\n";
  content += "-------------------------\n";
  content += "Registered IP Addresses: \n";
  content += "Python: " + pythonAddress + "\n";
  content += "Uno R4: " + unoR4Address + "\n";
  content += "=========================";
  sendToPython(content);
}
void updateSpeed(int value) {
  speed = value;
  dutyCycleA = value;
  dutyCycleB = value;
  dutyCycleC = value;
  dutyCycleD = value;
  ledcWrite(ENABLE_A_PIN, dutyCycleA);
  ledcWrite(ENABLE_B_PIN, dutyCycleB);
  ledcWrite(ENABLE_C_PIN, dutyCycleC);
  ledcWrite(ENABLE_D_PIN, dutyCycleD);
}
// ===================== PROCESS COMMAND =====================
void processCommand(int cmd) {
  if (emergency_stop) {
    return;
  }
  log("Received Command: " + String(cmd));
  time_since_last_command = millis();
  autostop_enable = true;
  switch (cmd) {
    case 0:  // Stop
      log("Stopping...", 'i');
      stopAll();
      break;
    case 1:  // Forward
      log("Moving Forward...", 'i');
      forward();
      break;
    case 2:  // Backward
      log("Moving Backward...", 'i');
      backward();
      break;
    case 3:  // Turn Left
      log("Turning Left...", 'i');
      turnLeft();
      break;
    case 4:  // Turn Right
      log("Turning Right...", 'i');
      turnRight();
      break;
    case 5:  // Strafe Left
      log("Strafing Left...", 'i');
      strafeLeft();
      break;
    case 6:  // Strafe Right
      log("Strafing Right...", 'i');
      strafeRight();
      break;
    default:
      log("Invalid Command! Stopping...", 'i');
      stopAll();
      break;
  }
}
void processAdminCommand(String cmd) {
  int value = cmd.substring(2).toInt(); // parse value starting at char index 2
  char action = cmd.charAt(0); // select action based on first char
  if (cmd.length() > 2 && cmd.charAt(1) != ' ') {
    return; // malformed input
  }
  switch (action) {
    case 'e':
      stopAll();
      emergency_stop = true;
      log("EMERGENCY STOPPING!! - reset ESP to re-enable movement commands", 0);
      break;
    case 'i':
      sendReport();
      log("Report Sent");
      break;
    case 's':
      updateSpeed(value);
      log("Speed updated to " + String(value));
      break;
    case 't':
      timer_delay = value;
      log("Timer delay updated to " + String(value));
      break;
    case 'd':
      log("Setting debug level to " + String(max(0, min(4, value))));
      debug_level = max(0, min(4, value));
      break;
  }
}
void registerIP(String id, String ip) {
  if (id == "python") {
    pythonAddress = ip;
    log("Set registered Python ip address to " + ip, 1);
  }
  else if (id == "uno r4") {
    unoR4Address = ip;
    log("Set registered Uno R4 ip address to " + ip, 1);
  }
  else {
    log("Unrecognized IP Address identifier: '" + id + "'. Available identifieres: 'python', uno r4'", 1);
  }
}
void readInput() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Allocate buffer to store received data
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = '\0';  // Null-terminate the string
    }
    log("Received message: " + String(incomingPacket));
    log(udp.remoteIP().toString());
    if (isDigit(incomingPacket[0])) {
      // Convert received data to an integer and call the respective function
      int cmd = atoi(incomingPacket); 
      processCommand(cmd);
    }
    else if (incomingPacket[0] == 'r') {
      registerIP(String(incomingPacket).substring(2), udp.remoteIP().toString());
    }
    else {
      processAdminCommand(String(incomingPacket));
    }
  }
}
// FreeRTOS task handle
TaskHandle_t udpTaskHandle = NULL;
void udpTask(void *params) {
  while (true) {
    readInput();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void setup() {
  Serial.begin(115200);
  Serial.println("Starting WiFi Rover...");
  // ----- MOTOR PIN SETUP -----
  pinMode(MOTOR_A_PIN_1, OUTPUT);
  pinMode(MOTOR_A_PIN_2, OUTPUT);
  pinMode(ENABLE_A_PIN, OUTPUT);
  pinMode(MOTOR_B_PIN_1, OUTPUT);
  pinMode(MOTOR_B_PIN_2, OUTPUT);
  pinMode(ENABLE_B_PIN, OUTPUT);
  pinMode(MOTOR_C_PIN_1, OUTPUT);
  pinMode(MOTOR_C_PIN_2, OUTPUT);
  pinMode(ENABLE_C_PIN, OUTPUT);
  pinMode(MOTOR_D_PIN_1, OUTPUT);
  pinMode(MOTOR_D_PIN_2, OUTPUT);
  pinMode(ENABLE_D_PIN, OUTPUT);
  // ----- PWM Setup (Depending on your ESP32 core you may need ledcSetup+ledcAttachPin) -----
  ledcAttachChannel(ENABLE_A_PIN, FREQUENCY_A, RESOLUTION_A, PWM_CHANNEL_A);
  ledcAttachChannel(ENABLE_B_PIN, FREQUENCY_B, RESOLUTION_B, PWM_CHANNEL_B);
  ledcAttachChannel(ENABLE_C_PIN, FREQUENCY_C, RESOLUTION_C, PWM_CHANNEL_C);
  ledcAttachChannel(ENABLE_D_PIN, FREQUENCY_D, RESOLUTION_D, PWM_CHANNEL_D);
  // ----- Set motor speeds -----
  ledcWrite(ENABLE_A_PIN, dutyCycleA);
  ledcWrite(ENABLE_B_PIN, dutyCycleB);
  ledcWrite(ENABLE_C_PIN, dutyCycleC);
  ledcWrite(ENABLE_D_PIN, dutyCycleD);

  // ----- WiFi Setup -----
  WiFi.softAP(ssid, password); // Set up ESP32 as an access point
  IPAddress IP = WiFi.softAPIP();
  espAddress = String(IP);
  Serial.println("Access Point IP: " + IP.toString());
  udp.begin(localUdpPort); // Start UDP server
  delay(1000);
  // Create periodic interrupt for receiving udp packets
  xTaskCreate(udpTask, "UDP Task", 4096, NULL, 1, &udpTaskHandle); // Task function, task name, stack size, task parameters, priority, task handle
  log("Rover ready.", 1);
}

void loop() {
  // Check for Autostop conditions
  if (autostop_enable && millis() - timer_delay > time_since_last_command) {
    log("Autostopping!", 'n');
    stopAll();
    autostop_enable = false;
  }
}

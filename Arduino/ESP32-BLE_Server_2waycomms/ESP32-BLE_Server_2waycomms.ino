/*
   LHNT Rover + BLE Control (No WiFi)
   Using Correct Motor Control Functions
   -------------------------------------------------
   Receives commands "0" through "6" over BLE:
   0 -> Stop
   1 -> Forward
   2 -> Backward
   3 -> Turn Left
   4 -> Turn Right
   5 -> Strafe Left
   6 -> Strafe Right
   Written by Ibrahim & Vic, modified for BLE by HSI
   January 26th 2025 - 4:29 PM
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
// ========== BLE SERVICE/CHARACTERISTIC UUIDs (Generate your own if you wish) ==========
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define INPUT_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define REPORT_CHARACTERISTIC_UUID "2d21fb45-ea18-4b00-9fc9-53e55e2363be"
#define NOTIFY_CHARACTERISTIC_UUID  "a654858a-5cbc-4bde-9f57-5bb967bd25b0"
// ========== Motor Pin Definitions (Same as your “correct” code) ==========
const int MOTOR_A_PIN_1 = 27;
const int MOTOR_A_PIN_2 = 26;
const int ENABLE_A_PIN  = 14;
const int MOTOR_B_PIN_1 = 25;
const int MOTOR_B_PIN_2 = 33;
const int ENABLE_B_PIN  = 32;
const int MOTOR_C_PIN_1 = 18;
const int MOTOR_C_PIN_2 = 5;
const int ENABLE_C_PIN  = 19;
const int MOTOR_D_PIN_1 = 4;
const int MOTOR_D_PIN_2 = 2;
const int ENABLE_D_PIN  = 15;
// ========== Motor PWM Settings ==========
const int FREQUENCY_A   = 30000;
const int PWM_CHANNEL_A = 0;
const int RESOLUTION_A  = 8;
int dutyCycleA          = 190;
const int FREQUENCY_B   = 30000;
const int PWM_CHANNEL_B = 0; // Using same channel as an example, but you can change to 1,2,3 if desired.
const int RESOLUTION_B  = 8;
int dutyCycleB          = 190;
const int FREQUENCY_C   = 30000;
const int PWM_CHANNEL_C = 0;
const int RESOLUTION_C  = 8;
int dutyCycleC          = 190;
const int FREQUENCY_D   = 30000;
const int PWM_CHANNEL_D = 0;
const int RESOLUTION_D  = 8;
int dutyCycleD          = 190;

// BLE Characteristic Pointers
BLECharacteristic *pInputCharacteristic;
BLECharacteristic *pReportCharacteristic;
BLECharacteristic *pNotifyCharacteristic;

// BLE Programmable Global Variables (and their default values)
int timer_delay = 200;    // set with 't' command 
int speed = 190;          // set with 's' command
bool enable_notifs = 1;   // set with 'n' command
int timer = 0; // autostop

// Other non-action BLE Commands
// send report            // request with 'r' command

// ===================== MOTOR CONTROL FUNCTIONS (From Your Correct Code) =====================
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
// ===================== BROADCAST INFO =====================
void sendReport() {
  String content = "==== LHNT Rover Info ====\n";
  content += "Timer Delay: " + String(timer_delay) + "\n";
  content += "Speed: " + String(speed) + "\n";
  content += "Notification Enable: " + String(enable_notifs) + "\n";
  content += "=========================";
  pReportCharacteristic->setValue(content.c_str());
}
void sendNotif(String message) {
  if (enable_notifs) {
    pNotifyCharacteristic->setValue(message.c_str());
    pNotifyCharacteristic->notify();
  }
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
  Serial.print("Received Command: ");
  Serial.println(cmd);
  switch (cmd) {
    case 0:  // Stop
      Serial.println("Stopping...");
      stopAll();
      break;
    case 1:  // Forward
      Serial.println("Moving Forward...");
      forward();
      timer = timer_delay;
      break;
    case 2:  // Backward
      Serial.println("Moving Backward...");
      backward();
      timer = timer_delay;
      break;
    case 3:  // Turn Left
      Serial.println("Turning Left...");
      turnLeft();
      timer = timer_delay;
      break;
    case 4:  // Turn Right
      Serial.println("Turning Right...");
      turnRight();
      timer = timer_delay;
      break;
    case 5:  // Strafe Left
      Serial.println("Strafing Left...");
      strafeLeft();
      timer = timer_delay;
      break;
    case 6:  // Strafe Right
      Serial.println("Strafing Right...");
      strafeRight();
      timer = timer_delay;
      break;
    default:
      Serial.println("Invalid Command! Stopping...");
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
    case 'r':
      sendReport();
      break;
    case 's':
      updateSpeed(value);
      break;
    case 't':
      timer_delay = value;
      break;
    case 'n':
      enable_notifs = (value > 0);
      break;
  }
}
// ===================== BLE CALLBACKS =====================
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    // Convert std::string to Arduino String
    String rxValue = String(pCharacteristic->getValue().c_str());
    if (rxValue.length() > 0) {
      // Attempt to parse the integer command
      if (isDigit(rxValue.charAt(0))) {
        int cmd = rxValue.toInt();
        processCommand(cmd);
      }
      else {
        try {
          processAdminCommand(rxValue);
        } catch (int exception2) {
          Serial.println("Invalid/Malformed Input!");
        }
      }
    }
  }
};
// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Rover...");
  // ----- MOTOR PIN SETUP -----
  pinMode(MOTOR_A_PIN_1, OUTPUT);
  pinMode(MOTOR_A_PIN_2, OUTPUT);
  pinMode(ENABLE_A_PIN,   OUTPUT);
  pinMode(MOTOR_B_PIN_1, OUTPUT);
  pinMode(MOTOR_B_PIN_2, OUTPUT);
  pinMode(ENABLE_B_PIN,   OUTPUT);
  pinMode(MOTOR_C_PIN_1, OUTPUT);
  pinMode(MOTOR_C_PIN_2, OUTPUT);
  pinMode(ENABLE_C_PIN,   OUTPUT);
  pinMode(MOTOR_D_PIN_1, OUTPUT);
  pinMode(MOTOR_D_PIN_2, OUTPUT);
  pinMode(ENABLE_D_PIN,   OUTPUT);
  // ----- PWM Setup (Depending on your ESP32 core you may need ledcSetup+ledcAttachPin) -----
  ledcAttachChannel(ENABLE_A_PIN, FREQUENCY_A, RESOLUTION_A, PWM_CHANNEL_A);
  ledcAttachChannel(ENABLE_B_PIN, FREQUENCY_B, RESOLUTION_B, PWM_CHANNEL_B);
  ledcAttachChannel(ENABLE_C_PIN, FREQUENCY_C, RESOLUTION_C, PWM_CHANNEL_C);
  ledcAttachChannel(ENABLE_D_PIN, FREQUENCY_D, RESOLUTION_D, PWM_CHANNEL_D);
  // Set motor speeds
  ledcWrite(ENABLE_A_PIN, dutyCycleA);
  ledcWrite(ENABLE_B_PIN, dutyCycleB);
  ledcWrite(ENABLE_C_PIN, dutyCycleC);
  ledcWrite(ENABLE_D_PIN, dutyCycleD);
  // ----- BLE Setup -----
  const char* deviceName = "ESP32-BLE-Rover";
  BLEDevice::init(deviceName);
  // Print device info
  Serial.print("Device Name: ");
  Serial.println(deviceName);
  Serial.print("ESP32 MAC Address: ");
  Serial.println(BLEDevice::getAddress().toString().c_str());
  // Create BLE Server & Service
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // Create BLE Characteristic
  pInputCharacteristic = pService->createCharacteristic(
                                        INPUT_CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                      );
  pReportCharacteristic = pService->createCharacteristic(
                                        REPORT_CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                      );
  pNotifyCharacteristic = pService->createCharacteristic(
                                        NOTIFY_CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE |
                                        BLECharacteristic::PROPERTY_NOTIFY |
                                        BLECharacteristic::PROPERTY_INDICATE
                                      );
  // Set initial value and callbacks
  pInputCharacteristic->setValue("Awaiting Command");
  pInputCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  pReportCharacteristic->setValue("Hello World! LHNT ESP32 BLE Communications");
  pNotifyCharacteristic->setValue("No notifications");
  pNotifyCharacteristic->addDescriptor(new BLE2902());

  // Start the service and begin advertising
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Helps with iOS BLE
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLE Advertising started. (Send '0'..'6' to control the rover)");
}
// ===================== LOOP =====================
void loop() {
  // All BLE handling is done via callbacks; nothing needed here
  while (timer > 0) {
    delay(1);
    timer--;
    if (timer == 0) {
      sendNotif("Autostopping!");
      stopAll();
    }
  }
}



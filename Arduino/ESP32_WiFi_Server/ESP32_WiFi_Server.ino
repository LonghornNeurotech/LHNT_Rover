/*
  Written by Ibrahim & Vic
  November 7th, 2024
  LHNT Rover Fall 2024
*/

// this code uses 2 motor drivers, has 7 possible degrees of freedom (forward, backward, left, right, strafe left, strafe right, stop)
// does not automatically stop the rover after a given time interval. 

// From HSI
#include <WiFi.h>
#include <WiFiUdp.h>
const char* ssid = "ESP32-LHNT";
const char* password = "longhorns";
WiFiUDP udp;
const int localUdpPort = 1234;  // Port for receiving




int command = 0;




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








































void processCommand() {
  Serial.print("Recieved: ");
  Serial.println(command);
  if (command == 1) {
    Serial.println("Moving forward.");
    forward();
  }
  else if (command == 0) {
    Serial.println("Stopping.");
    stopAll();
  }
  else if (command == 2) {
    Serial.println("Moving backward.");
    backward();
  }
  else if (command == 3) {
    Serial.println("Turning left.");
    turnLeft();
  }
  else if (command == 4) {
    Serial.println("Turning right.");
    turnRight();
  }


  else if (command == 5) {
    Serial.println("Strafing left.");
    strafeLeft();
  }
  else if (command == 6) {
    Serial.println("Strafing right.");
    strafeRight();
  }




}












void readInput() {
  // From HSI
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Allocate buffer to store received data
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = '\0';  // Null-terminate the string
    }
    Serial.print("Received message: ");
    Serial.println(incomingPacket);
    // Convert received data to an integer and call the respective function
    command = atoi(incomingPacket);
    processCommand();
  }
}




void setup() {
  Serial.begin(115200);




  // Setting pin modes
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
 
  // configure LEDC PWM
  ledcAttachChannel(ENABLE_A_PIN, FREQUENCY_A, RESOLUTION_A, PWM_CHANNEL_A);
  ledcAttachChannel(ENABLE_B_PIN, FREQUENCY_B, RESOLUTION_B, PWM_CHANNEL_B);
  ledcAttachChannel(ENABLE_C_PIN, FREQUENCY_C, RESOLUTION_C, PWM_CHANNEL_C);
  ledcAttachChannel(ENABLE_D_PIN, FREQUENCY_D, RESOLUTION_D, PWM_CHANNEL_D);




  // set speed of motors
  ledcWrite(ENABLE_A_PIN, dutyCycleA);
  ledcWrite(ENABLE_B_PIN, dutyCycleB);
  ledcWrite(ENABLE_C_PIN, dutyCycleC);
  ledcWrite(ENABLE_D_PIN, dutyCycleD);




  // From HSI
  // Set up ESP32 as an access point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP: ");
  Serial.println(IP);
  // Start UDP server
  udp.begin(localUdpPort);




  Serial.print("Rover ready.");
}




void loop() {
  // check for new serial monitor input
  readInput();
}

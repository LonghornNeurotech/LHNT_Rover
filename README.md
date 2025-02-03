# LHNT's Rover Project

## Table of Contents

1. [Project Overview](#Project-Overview)
2. [Team](#Team)
3. [Features](#Features)

---

## Project Overview

The rover project at LHNT aims to build a rover from scratch, combining efforts from the electronics, drivetrain, and hardware-software integration teams. The rover will have six degrees of freedom: moving forward, backward, turning left or right, and strafing left or right. It will communicate wirelessly with a computer that determines its movements. Ultimately, the rover will achieve free motion, controlled entirely by the user's neural data.

## Team

- **Electronics Team**
  - Design the rover's circuit and code to be run on the ESP32.
  - Team Lead: Victor Moran
  - Members: Ibrahim Aksoy, Aarav Lakdawala
- **Drivetrain Team**
  - Build the rover's body, shell, and obstacles for testing.
  - Team Lead: Sydney Fawcett
  - Members: Nandini Senthilkumar, Graham Gilbert, Matthew Wilson
- **Hardware-Software Integration Team**
  - Ensure the rover can wirelessly communicate with a computer, and ensure the reliability of this connection.
  - Team Lead: Sierra Nguyen
  - Members: Rishik Boddeti, Michael Chung, Aashvi Maithani

## Features

### Implemented Features

1. #### Wireless Communication using WiFi
     - The ESP32 is acting as a server which a computer can connect to using WiFi. Once connected, the computer can run the python code and control the rover. 
2. #### Up to Six Degrees of Freedom
     - The rover can move in 6 ways: forward, backward, turn left, turn right, strafe left, and strafe right. Not all of these movements are necessary for the rover's operation, so the user is able to choose which oens to use.
3. #### Control Speed and Runtime
     - Using python, you can control the rover's speed and runtime (time before it stops automatically).
4. #### Mecanum Wheels
    - The rover's wheels are mecanum wheels. This allows the rover to strafe left or right.
5. #### Wireless Communication using BlueTooth Low Energy
     - We can also communicate with the rover using BLE.

### Planned Features

1. #### Obstacles
     - Obstacles will make the testing process easier, but will also be cool for presentation purposes. 
2. #### 3D Printed Shell
     - Putting a shell on top of the rover will make it look nicer. :D

## Controls
0 - Stop

1 - Forward

2 - Backward

3 - Turn Left

4 - Turn Right

5 - Strafe Left

6 - Strafe Right

r - Request Report

s \[num\] - set speed to num (0 to 255)

t \[num\] - set timer delay to num (in ms)

n - enable notifications


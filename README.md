# LumiRover Prototype 🚜❄️

## Overview
LumiRover is a 4-wheel drive, Bluetooth-controlled scale prototype for a snow removal robot. Developed for a University Mechatronics Project course, it features omni-directional drive kinematics, a motorized plow, and electronic safety systems. The project has been completed but I want to take this as a personal project and scale it up to an autonomous snow removal robot.

## Features
* **Custom Mobile App Interface:** Controlled via a custom MIT App Inventor application with a "Dead Man's Switch" safety protocol.
* **Mecanum Kinematics:** True omni-directional strafing utilizing 4 independent DC motors.
* **Two-Tier Safety System:** Software-level mechanical stall detection (Virtual Fuse) paired with hardware E-Stops.
* **Visual Telemetry:** Grove LED state machine for real-time visual system diagnostics (Standby, Working, Stalled).

## Hardware Architecture
* **Microcontroller:** Arduino Mega 2560
* **Drivetrain:** 4x 6V TT Gear Motors
* **Motor Drivers:** 2x TB6612FNG (Wheels), 1x BTS7960 (Spreader)
* **Actuators:** Grove Servo (Plow Lift), 24V DC Motor (Spreader)
* **Connectivity:** HC-05 Bluetooth Module
* **Power:** 11.1V LiPo Battery with LM2596 Buck Converters for 5V logic isolation.

## Who It's For
This project is built for hobbyist roboticists, embedded C++ developers, and mechatronics enthusiasts looking for an open-source reference architecture for holonomic 4WD rovers transitioning from manual Bluetooth teleoperation to autonomous navigation.

## Tech Stack & Hardware Architecture
- **Firmware / Embedded:** C++ (Arduino Framework, migrating to PlatformIO)
- **Microcontroller:** Arduino Uno / ATmega328P
- **Actuation:** 4x DC Geared Motors with Mecanum wheels driven via L298N Dual H-Bridge Motor Drivers
- **Sensors:** HC-SR04 Ultrasonic Sensor (distance/obstacle detection), Infrared/Flame Sensor (hazard detection)
- **Communication:** HC-05 / HC-06 Bluetooth Serial Module (UART teleoperation protocol)

## Getting Started (How to Build & Flash)

### Prerequisites
- [Arduino IDE](https://www.arduino.cc/en/software) (v2.0+) or [PlatformIO Core (CLI)](https://platformio.org/)
- USB-A to USB-B cable for serial flashing

### Installation & Flashing
1. Clone this repository:
   ```bash
   git clone https://github.com/Akinlabi-Mechatronics-Biomed/LumiRover-Mechatronics.git
   cd LumiRover-Mechatronics
2. Open LumiRover.ino in the Arduino IDE.
3. Select Tools > Board > Arduino AVR Boards > Arduino Uno.
4. Select your connected serial port under Tools > Port.
5. Disconnect the Bluetooth RX/TX pins temporarily (if connected to hardware pins 0 and 1) to prevent UART upload collisions, then click Upload.
6. Reconnect the Bluetooth module and pair with the rover using a Bluetooth Serial terminal (9600 baud).

## Software Roadmap
The project is actively transitioning from a single-file prototype (LumiRover.ino) into a modular, testable C++ embedded framework with a companion Python desktop control and simulation suite.

### Phase 1: Embedded C++ Refactoring & PlatformIO Migration

Migrate repository structure from single .ino sketch to a standard PlatformIO C++ project (platformio.ini, src/, include/, lib/, test/).
Decouple hardware logic into modular C++ classes: MecanumDrive, UltrasonicSensor, HazardMonitor, and BluetoothComms.
Replace blocking delay() calls with non-blocking millis() state machines for real-time sensor polling.

### Phase 2: Kinematics Engine, PID Control & Unit Testing
Implement full inverse kinematics math (vx, vy, omega -> individual wheel PWM vectors) instead of hardcoded directional states.
Add native C++ unit tests (using PlatformIO Unity framework) to verify kinematics math, sensor filtering (moving average / Kalman filter), and state transitions on host PC without hardware.
Add closed-loop PID controller classes for speed stabilization and heading hold.

### Phase 3: Autonomous Navigation & Coverage Path Planning
Implement a Finite State Machine (FSM) switching cleanly between MANUAL_TELEOP, AUTONOMOUS_SWEEP, OBSTACLE_AVOIDANCE, and EMERGENCY_STOP.
Build a boustrophedon (back-and-forth grid) snow-clearing path planner with dead-reckoning estimation.

### Phase 4: Python Desktop Telemetry Dashboard & Hardware-in-the-Loop Simulator
Build a Python CLI/GUI telemetry client (pyserial / bleak) to send structured packet commands (JSON/binary with CRC checksums) and log real-time sensor telemetry.
Create a 2D Python simulator (pygame / matplotlib) to visualize and test LumiRover's Mecanum kinematics and obstacle avoidance algorithms virtually.

## Author
* [Akinlabi]

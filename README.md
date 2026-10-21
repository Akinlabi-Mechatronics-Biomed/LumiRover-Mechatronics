# LumiRover Prototype 🚜❄️

## Overview
LumiRover is a 4-wheel drive, Bluetooth-controlled scale prototype for an autonomous snow removal robot. Developed for a University Mechatronics Project course, it features omni-directional drive kinematics, a motorized plow, and electronic safety systems.

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

# LumiRover-Mechatronics

LumiRover is a 4-wheel drive, Bluetooth-controlled omnidirectional scale prototype for an autonomous snow-clearing and utility robotics platform. It uses Mecanum wheels for holonomic kinematics (forward, reverse, lateral strafing, and zero-radius rotation) alongside active ultrasonic obstacle avoidance and thermal/IR hazard detection.

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

## Author
* [Akinlabi]

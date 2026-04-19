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

## Author
* [Akinlabi]

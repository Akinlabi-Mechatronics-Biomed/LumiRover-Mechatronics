#ifndef LUMIROVER_CONFIG_H
#define LUMIROVER_CONFIG_H

#include <Arduino.h>

// ============================================================================
// LumiRover Hardware Pinout & Control Constants
// ============================================================================

namespace LumiConfig {

    // Serial & Bluetooth Communication
    constexpr unsigned long SERIAL_BAUD_RATE = 9600;
    constexpr unsigned long TELEMETRY_INTERVAL_MS = 100; // 10 Hz

    // Ultrasonic Sensor (HC-SR04) Pins & Thresholds
    constexpr uint8_t PIN_ULTRASONIC_TRIG = A0;
    constexpr uint8_t PIN_ULTRASONIC_ECHO = A1;
    constexpr uint16_t OBSTACLE_STOP_DISTANCE_CM = 20;
    constexpr uint16_t OBSTACLE_SLOW_DISTANCE_CM = 40;
    constexpr unsigned long ULTRASONIC_TIMEOUT_US = 25000; // ~4.2m max range

    // Hazard / Flame / IR Sensor
    constexpr uint8_t PIN_HAZARD_SENSOR = A2;
    constexpr int HAZARD_TRIGGER_THRESHOLD = 300;

    // Front-Left (FL) & Rear-Left (RL) Motor Driver Pins
    constexpr uint8_t PIN_MOTOR_FL_IN1 = 2;
    constexpr uint8_t PIN_MOTOR_FL_IN2 = 3;
    constexpr uint8_t PIN_MOTOR_RL_IN3 = 4;
    constexpr uint8_t PIN_MOTOR_RL_IN4 = 5;

    // Front-Right (FR) & Rear-Right (RR) Motor Driver Pins
    constexpr uint8_t PIN_MOTOR_FR_IN1 = 6;
    constexpr uint8_t PIN_MOTOR_FR_IN2 = 7;
    constexpr uint8_t PIN_MOTOR_RR_IN3 = 8;
    constexpr uint8_t PIN_MOTOR_RR_IN4 = 9;

    // PWM Speed Settings (0 - 255)
    constexpr uint8_t SPEED_MAX = 255;
    constexpr uint8_t SPEED_CRUISE = 180;
    constexpr uint8_t SPEED_STRAFE = 200;
    constexpr uint8_t SPEED_PRECISION = 120;

    // Operating States (for upcoming FSM implementation)
    enum class RoverState : uint8_t {
        IDLE = 0,
        MANUAL_TELEOP = 1,
        AUTONOMOUS_SWEEP = 2,
        OBSTACLE_AVOIDANCE = 3,
        EMERGENCY_HAZARD_STOP = 4
    };

} // namespace LumiConfig

#endif // LUMIROVER_CONFIG_H

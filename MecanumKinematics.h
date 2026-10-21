#ifndef MECANUM_KINEMATICS_H
#define MECANUM_KINEMATICS_H

#include <Arduino.h>

// ============================================================================
// Mecanum Inverse Kinematics Engine
// Converts normalized chassis velocities (vx, vy, omega) into individual
// wheel PWM speeds for 4WD omnidirectional movement.
// ============================================================================

struct WheelSpeeds {
    int16_t frontLeft;
    int16_t frontRight;
    int16_t rearLeft;
    int16_t rearRight;
};

class MecanumKinematics {
public:
    /**
     * @brief Compute individual wheel speeds from desired robot velocity vectors.
     * @param vx Forward/reverse velocity normalized in [-1.0, 1.0]
     * @param vy Lateral strafe velocity normalized in [-1.0, 1.0] (right is positive)
     * @param omega Rotational velocity normalized in [-1.0, 1.0] (CW is positive)
     * @param maxPwm Maximum PWM duty cycle (default 255)
     * @return WheelSpeeds Signed PWM values in [-maxPwm, maxPwm] for each wheel
     */
    static WheelSpeeds computeWheelPWM(float vx, float vy, float omega, uint8_t maxPwm = 255) {
        // Inverse kinematics for X-configuration Mecanum wheels
        float fl = vx + vy + omega;
        float fr = vx - vy - omega;
        float rl = vx - vy + omega;
        float rr = vx + vy - omega;

        // Normalize wheel speeds if any vector exceeds magnitude 1.0
        float maxMagnitude = max(max(abs(fl), abs(fr)), max(abs(rl), abs(rr)));
        if (maxMagnitude > 1.0f) {
            fl /= maxMagnitude;
            fr /= maxMagnitude;
            rl /= maxMagnitude;
            rr /= maxMagnitude;
        }

        WheelSpeeds speeds;
        speeds.frontLeft  = static_cast<int16_t>(fl * maxPwm);
        speeds.frontRight = static_cast<int16_t>(fr * maxPwm);
        speeds.rearLeft   = static_cast<int16_t>(rl * maxPwm);
        speeds.rearRight  = static_cast<int16_t>(rr * maxPwm);
        return speeds;
    }
};

#endif // MECANUM_KINEMATICS_H

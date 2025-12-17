#include <iostream>
#include <cmath>
#include <stdexcept>

// Structure to hold our three motor angles in degrees
struct RobotConfig {
    double base;     // Phi
    double shoulder; // Theta
    double elbow;    // Alpha
};

const double degToRad = M_PI / 180.0;

// Define physical limits for the motors (in radians
const double BASE_MIN = -95.0 * degToRad;
const double BASE_MAX = 90.0 * degToRad;
const double SHOULDER_MIN = degToRad;
const double SHOULDER_MAX = degToRad;
const double ELBOW_MIN = degToRad;
const double ELBOW_MAX = degToRad;

RobotConfig calculate3DIK(double x, double y, double z, double L1, double L2) {
    // 1. Calculate Base Rotation (Phi)
    // This points the arm's "plane" toward the target (x, y)
    double baseRad = std::atan2(y, x);

    // 2. Simplify to a 2D "elevation plane"
    // Calculate horizontal distance from base to target
    double rPlanar = std::sqrt(x * x + y * y);
    // Calculate total 3D distance from base (0,0,0) to target (x,y,z)
    double R = std::sqrt(rPlanar * rPlanar + z * z);

    // 3. Reachability Check
    if (R > (L1 + L2) || R < std::abs(L1 - L2)) {
        throw std::runtime_error("Target point is physically unreachable!");
    }

    // 4. Solve for Elbow Angle (Alpha) using Law of Cosines
    double cosAlpha = (L1 * L1 + L2 * L2 - R * R) / (2 * L1 * L2);
    double elbowRad = std::acos(cosAlpha);

    // 5. Solve for Shoulder Angle (Theta)
    // Part A: Elevation from ground to target
    double thetaElevation = std::atan2(z, rPlanar);
    
    // Part B: Internal triangle angle at the shoulder
    double cosThetaInternal = (L1 * L1 + R * R - L2 * L2) / (2 * L1 * R);
    double thetaInternal = std::acos(cosThetaInternal);

    // Combine for elbow-down configuration
    double shoulderRad = thetaElevation - thetaInternal;
    
    // Check joint limits
    if (shouderRad < SHOULDER_MIN || shoulderRad > SHOULDER_MAX) {
        throw std::runtime_error("Target would exceed shoulder limits");
    }
    if (elbowRad  < ELBOW_MIN || elbowRad > ELBOW_MAX) {
        throw std::runtime_error("Target would exceed elbow limits");
    }
    if (baseRad   < BASE_MIN || baseRad > BASE_MAX) {
        throw std::runtime_error("Target would exceed elbow limits");
    }

    // 6. Convert all results to Degrees
    const double radToDeg = 180.0 / M_PI;
    return {
        baseRad * radToDeg,
        shoulderRad * radToDeg,
        elbowRad * radToDeg
    };
}
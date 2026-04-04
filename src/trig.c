/**
 * @file trig.c
 * @brief Trigonometry: Angles
 * @author Justin Thoreson
 */

#include <trig.h>
#include <math.h>

/**
 * @brief Angle of degrees denoting a half circle of rotation.
 */
static const float HALF_CIRCLE_DEGREES = 180.0f;

float angle_radians(const float degrees) {
	return (degrees / HALF_CIRCLE_DEGREES) * M_PI;
}

float angle_degrees(const float radians) {
	return (radians / M_PI) * HALF_CIRCLE_DEGREES;
}

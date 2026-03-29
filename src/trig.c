/**
 * @file trig.c
 * @brief Trigonometry: Angles
 * @author Justin Thoreson
 */

#include <trig.h>
#include <math.h>

static const float FULL_CIRCLE_DEGREES = 180.0f;

float angle_radians(const float degrees) {
	return (degrees / FULL_CIRCLE_DEGREES) * M_PI;
}

float angle_degrees(const float radians) {
	return (radians / M_PI) * FULL_CIRCLE_DEGREES;
}

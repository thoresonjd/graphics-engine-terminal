/**
 * @file trig.h
 * @brief Trigonometry: Angles
 * @author Justin Thoreson
 */

#pragma once
#ifndef TRIG_H
#define TRIG_H

/**
 * @brief Convert from degrees to radians.
 * @param[in] degrees An angle measured in degrees.
 * @return The corresponding angle measured in radians.
 */
float angle_radians(const float degrees);

/**
 * @brief Convert from radians to degrees.
 * @param[in] radians An angle measured in radians.
 * @return The corresponding angle measured in degrees.
 */
float angle_degrees(const float radians);

#endif // TRIG_H

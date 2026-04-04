/**
 * @file camera.h
 * @brief Camera controls
 * @author Justin Thoreson
 */

#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <linalg.h>

/**
 * @brief The camera type.
 */
typedef struct camera_t camera_t;

/**
 * @brief Directions of camera movement.
 */
typedef enum {
	CAMERA_DIRECTION_FORWARD,
	CAMERA_DIRECTION_BACKWARD,
	CAMERA_DIRECTION_LEFT,
	CAMERA_DIRECTION_RIGHT,
	CAMERA_DIRECTION_UP,
	CAMERA_DIRECTION_DOWN
} camera_direction_t;

/**
 * @brief Initialize the camera.
 * @param[in] position The camera's position.
 * @param[in] front The front of the camera.
 * @param[in] world_up Axis pointing upward in world space.
 * @param[in] fov The camera's field of view.
 * @return The camera.
 */
camera_t* camera_init(
	const vec3f_t position,
	const vec3f_t front,
	const vec3f_t world_up,
	const float fov
);

/**
 * @brief Deallocate the camera.
 * @param[in] camera The camera to destroy
 */
void camera_destroy(camera_t* camera);

/**
 * @brief Compute and retrieve the view matrix of the camera.
 * @param[in] camera The camera to compute the view matrix of.
 * @param[out] out_view_matrix The output view matrix. 
 */
void camera_compute_view_matrix(
	const camera_t* const camera,
	mat4f_t out_view_matrix
);

/**
 * @brief Retrieve the camera's field of view.
 * @param[in] camera The camera to get the field of view for.
 */
float camera_get_fov(const camera_t* const camera);

/**
 * @brief Move the camera by updating its position.
 *
 * Ideally, the move function is provided a change in time. That change in time
 * is multiplied by a constant value denoting the movement sensitivity to
 * determine the velocity. For now, the velocity is accepted directly.
 *
 * @param[in,out] camera The camera to move.
 * @param[in] direction The direction to move the camera.
 * @param[in] velocity The amount to move.
 */
void camera_move(
	camera_t* const camera,
	const camera_direction_t direction,
	const float velocity
);

/**
 * @brief Adjust the direction in which the camera is pointed.
 *
 * Ideally, the look function is provided offsets in the x-axis (pitch) and/or
 * y-axis (yaw), and those offsets are multiplied by a value denoting the look
 * sensitivity to determine how far to adjust the camera's viewing angle.
 *
 * Neither of the Euler angles are constrained and can undergo full rotations.
 *
 * @param[in,out] camera The camera to point.
 * @param[in] offset_pitch The amount to change the pitch in degrees.
 * @param[in] offset_yaw The amount to change the yaw in degrees.
 */
void camera_look(
	camera_t* const camera,
	const float offset_pitch,
	const float offset_yaw
);

#endif // CAMERA_H

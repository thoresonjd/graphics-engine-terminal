/**
 * @file camera.c
 * @brief Camera controls
 * @author Justin Thoreson
 */

#include <camera.h>
#include <linalg.h>
#include <trig.h>
#include <math.h>
#include <stdlib.h>

/**
 * @brief The camera type.
 */
struct camera_t {
	/**
	 * @brief The position of the camera.
	 */
	vec3f_t position;

	/**
	 * @brief The direction in which the camera points.
	 */
	vec3f_t front;

	/**
	 * @brief Axis pointing to the right relateive to the camera.
	 */
	vec3f_t right;

	/**
	 * @brief Axis pointing upward relative to the camera.
	 */
	vec3f_t up;

	/**
	 * @brief Axis pointing upward relative to world space.
	 */
	vec3f_t world_up;

	/**
	 * @brief The camera's field of view.
	 */
	float fov;

	/**
	 * @brief The vertical angle of the camera.
	 */
	float pitch;

	/**
	 * @brief The horizontal angle of the camera.
	 */
	float yaw;
};

/**
 * @brief The default camera pitch.
 */
static const float CAMERA_DEFAULT_PITCH = 0.0f;

/**
 * @brief The default camera yaw.
 */
static const float CAMERA_DEFAULT_YAW = -90.0f;

/**
 * @brief Compute vectors comprising the camera.
 * @param[in,out] camera The camera.
 */
static void camera_compute_vectors(camera_t* const camera);

camera_t* camera_init(
	const vec3f_t position,
	const vec3f_t front,
	const vec3f_t world_up,
	const float fov
) {
	camera_t* camera = (camera_t*)malloc(sizeof(camera_t));
	camera->position = position;
	camera->front = front;
	camera->world_up = world_up;
	camera->fov = fov;
	camera->pitch = CAMERA_DEFAULT_PITCH;
	camera->yaw = CAMERA_DEFAULT_YAW;
	camera_compute_vectors(camera);
	return camera;
}

void camera_destroy(camera_t* camera) {
	free(camera);
	camera = NULL;
}

void camera_compute_view_matrix(
	const camera_t* const camera,
	mat4f_t out_view_matrix
) {
	vec3f_t direction = vec3f_add(camera->position, camera->front);
	mat4f_look_at(out_view_matrix, camera->position, direction, camera->up);
}

float camera_get_fov(const camera_t* const camera) {
	return camera->fov;
}

void camera_move(
	camera_t* const camera,
	const camera_direction_t direction,
	const float velocity
) {
	vec3f_t* const position = &camera->position;
	vec3f_t* const front = &camera->front;
	vec3f_t* const right = &camera->right;
	vec3f_t* const up = &camera->up;
	switch(direction) {
		case CAMERA_DIRECTION_FORWARD: {
			const vec3f_t distance = vec3f_float_multiply(*front, velocity);
			*position = vec3f_add(*position, distance);
			break;
		}
		case CAMERA_DIRECTION_BACKWARD: {
			const vec3f_t distance = vec3f_float_multiply(*front, velocity);
			*position = vec3f_subtract(*position, distance);
			break;
		}
		case CAMERA_DIRECTION_LEFT: {
			const vec3f_t distance = vec3f_float_multiply(*right, velocity);
			*position = vec3f_subtract(*position, distance);
			break;
		}
		case CAMERA_DIRECTION_RIGHT: {
			const vec3f_t distance = vec3f_float_multiply(*right, velocity);
			*position = vec3f_add(*position, distance);
			break;
		}
		case CAMERA_DIRECTION_UP: {
			const vec3f_t distance = vec3f_float_multiply(*up, velocity);
			*position = vec3f_add(*position, distance);
			break;
		}
		case CAMERA_DIRECTION_DOWN: {
			const vec3f_t distance = vec3f_float_multiply(*up, velocity);
			*position = vec3f_subtract(*position, distance);
			break;
		}
	}
}

void camera_look(
	camera_t* const camera,
	const float offset_pitch,
	const float offset_yaw
) {
	camera->pitch += offset_pitch;
	camera->yaw += offset_yaw;
	camera_compute_vectors(camera);
}

static void camera_compute_vectors(camera_t* const camera) {
	const float pitch_rad = angle_radians(camera->pitch);
	const float yaw_rad = angle_radians(camera->yaw);
	const float sin_pitch = sinf(pitch_rad);
	const float cos_pitch = cosf(pitch_rad);
	const float sin_yaw = sinf(yaw_rad);
	const float cos_yaw = cosf(yaw_rad);
	const vec3f_t direction = {
		.x = cos_pitch * cos_yaw,
		.y = sin_pitch,
		.z = cos_pitch * sin_yaw
	};
	camera->front = vec3f_normalize(direction);
	camera->right = vec3f_normalize(vec3f_cross(camera->front, camera->world_up));
	camera->up = vec3f_normalize(vec3f_cross(camera->right, camera->front));
}


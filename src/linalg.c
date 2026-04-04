/**
 * @file linalg.c
 * @brief Linear Algebra: Vectors & Matrices
 * @author Justin Thoreson
 */

#include <linalg.h>
#include <math.h>
#include <string.h>

void mat4f_multiply(
	mat4f_t out_matrix,
	const mat4f_t matrix_a,
	const mat4f_t matrix_b
) {
	mat4f_t multiplied = {{ 0 }};
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				multiplied[i][j] += matrix_a[i][k] * matrix_b[k][j];
	memcpy(out_matrix, multiplied, sizeof(mat4f_t));
}

void mat4f_scale(
	mat4f_t out_matrix,
	const mat4f_t matrix,
	const vec3f_t vector
) {
	memcpy(out_matrix, matrix, sizeof(mat4f_t));
	out_matrix[0][0] *= vector.x;
	out_matrix[1][1] *= vector.y;
	out_matrix[2][2] *= vector.z;
}

void mat4f_rotate(
	mat4f_t out_matrix,
	const mat4f_t matrix,
	const vec3f_t axis,
	const float angle
) {
	const float sin_theta = sinf(angle);
	const float cos_theta = cosf(angle);
	const float versin_theta = 1.0f - cos_theta;
	const vec3f_t norm_axis = vec3f_normalize(axis);
	const mat4f_t rotation = {	
		{
			norm_axis.x * norm_axis.x * versin_theta + cos_theta,
			norm_axis.x * norm_axis.y * versin_theta - norm_axis.z * sin_theta,
			norm_axis.x * norm_axis.z * versin_theta + norm_axis.y * sin_theta, 
			0.0f
		},
		{
			norm_axis.y * norm_axis.x * versin_theta + norm_axis.z * sin_theta,
			norm_axis.y * norm_axis.y * versin_theta + cos_theta,
			norm_axis.y * norm_axis.z * versin_theta - norm_axis.x * sin_theta,
			0.0f
		},
		{
			norm_axis.z * norm_axis.x * versin_theta - norm_axis.y * sin_theta,
			norm_axis.z * norm_axis.y * versin_theta + norm_axis.x * sin_theta,
			norm_axis.z * norm_axis.z * versin_theta + cos_theta,
			0.0f
		},
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	mat4f_multiply(out_matrix, rotation, matrix);
}

void mat4f_translate(
	mat4f_t out_matrix,
	const mat4f_t matrix,
	const vec3f_t vector
) {
	memcpy(out_matrix, matrix, sizeof(mat4f_t));
	out_matrix[0][3] += vector.x;
	out_matrix[1][3] += vector.y;
	out_matrix[2][3] += vector.z;
}

void mat4f_perspective(
	mat4f_t out_matrix,
	const float x_fov,
	const float aspect_ratio,
	const float z_near,
	const float z_far
) {
	const float cot_half_x_fov = 1.0f / tanf(x_fov / 2.0f);
	mat4f_t perspective = {
		{cot_half_x_fov, 0.0f, 0.0f, 0.0f},
		{0.0f, aspect_ratio * cot_half_x_fov, 0.0f, 0.0f},
		{0.0f, 0.0f, -(z_far + z_near) / (z_far - z_near), -(2.0f * z_far * z_near) / (z_far - z_near)},
		{0.0f, 0.0f, -1.0f, 0.0f}
	};
	memcpy(out_matrix, perspective, sizeof(mat4f_t));
}

void mat4f_look_at(
	mat4f_t out_matrix,
	const vec3f_t eye,
	const vec3f_t target,
	const vec3f_t world_up
) {
	const vec3f_t front = vec3f_normalize(vec3f_subtract(target, eye));
	const vec3f_t right = vec3f_normalize(vec3f_cross(front, world_up));
	const vec3f_t up = vec3f_cross(right, front);
	mat4f_t look_at = {
		{right.x, right.y, right.z, -vec3f_dot(right, eye)},
		{up.x, up.y, up.z, -vec3f_dot(up, eye)},
		{-front.x, -front.y, -front.z, vec3f_dot(front, eye)},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	memcpy(out_matrix, look_at, sizeof(mat4f_t));
}

vec4f_t vec4f_mat4f_multiply(const vec4f_t vector, const mat4f_t matrix) {
	return (vec4f_t){
		.x = vector.x * matrix[0][0] + vector.y * matrix[0][1] + vector.z * matrix[0][2] + vector.w * matrix[0][3],
		.y = vector.x * matrix[1][0] + vector.y * matrix[1][1] + vector.z * matrix[1][2] + vector.w * matrix[1][3],
		.z = vector.x * matrix[2][0] + vector.y * matrix[2][1] + vector.z * matrix[2][2] + vector.w * matrix[2][3],
		.w = vector.x * matrix[3][0] + vector.y * matrix[3][1] + vector.z * matrix[3][2] + vector.w * matrix[3][3]
	};
}

vec4f_t vec4f_add(const vec4f_t vector_a, const vec4f_t vector_b) {
	return (vec4f_t){
		vector_a.x + vector_b.x,
		vector_a.y + vector_b.y,
		vector_a.z + vector_b.z,
		vector_a.w + vector_b.w
	};
}

vec4f_t vec4f_subtract(const vec4f_t vector_a, const vec4f_t vector_b) {
	return (vec4f_t){
		vector_a.x - vector_b.x,
		vector_a.y - vector_b.y,
		vector_a.z - vector_b.z,
		vector_a.w - vector_b.w
	};
}

vec4f_t vec4f_float_divide(const vec4f_t vector, const float scalar) {
	return (vec4f_t){
		vector.x / scalar,
		vector.y / scalar,
		vector.z / scalar,
		vector.w / scalar
	};
}

vec3f_t vec3f_add(const vec3f_t vector_a, const vec3f_t vector_b) {
	return (vec3f_t){
		vector_a.x + vector_b.x,
		vector_a.y + vector_b.y,
		vector_a.z + vector_b.z
	};
}

vec3f_t vec3f_subtract(const vec3f_t vector_a, const vec3f_t vector_b) {
	return (vec3f_t){
		vector_a.x - vector_b.x,
		vector_a.y - vector_b.y,
		vector_a.z - vector_b.z
	};
}

vec3f_t vec3f_float_multiply(const vec3f_t vector, const float scalar) {
	return (vec3f_t){
		vector.x * scalar,
		vector.y * scalar,
		vector.z * scalar
	};
}

vec3f_t vec3f_float_divide(const vec3f_t vector, const float scalar) {
	if (scalar == 0.0f) // zero vector
		return vector;
	return (vec3f_t){
		vector.x / scalar,
		vector.y / scalar,
		vector.z / scalar
	};
}

float vec3f_magnitude(const vec3f_t vector) {
	return fabsf(sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z));
}

vec3f_t vec3f_normalize(const vec3f_t vector) {
	return vec3f_float_divide(vector, vec3f_magnitude(vector));
}

float vec3f_dot(const vec3f_t vector_a, const vec3f_t vector_b) {
	return vector_a.x * vector_b.x + vector_a.y * vector_b.y + vector_a.z * vector_b.z;
}

vec3f_t vec3f_cross(const vec3f_t vector_a, const vec3f_t vector_b) {
	return (vec3f_t){
		.x = vector_a.y * vector_b.z - vector_a.z * vector_b.y,
		.y = vector_a.z * vector_b.x - vector_a.x * vector_b.z,
		.z = vector_a.x * vector_b.y - vector_a.y * vector_b.x
	};
}

#if 0
/**
 * @brief Project a vector in three-dimensional space onto the two-dimensional
 *        screen.
 *
 * Projection is accomplished via perspective divide.
 *
 * v = <x, y, z> -> v' = <x/z, y/z, z>
 *
 * @param[in] vector A vertex to project.
 * @return The corresponding projected vector.
 */
vec3f_t vec3f_project(const vec3f_t vector) {
	return (vec3f_t){
		.x = vector.x / vector.z,
		.y = vector.y / vector.z,
		.z = vector.z
	};
}
#endif 


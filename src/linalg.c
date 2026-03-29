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

vec4f_t vec4f_mat4f_multiply(const vec4f_t vector, const mat4f_t matrix) {
	return (vec4f_t){
		.x = vector.x * matrix[0][0] + vector.y * matrix[0][1] + vector.z * matrix[0][2] + vector.w * matrix[0][3],
		.y = vector.x * matrix[1][0] + vector.y * matrix[1][1] + vector.z * matrix[1][2] + vector.w * matrix[1][3],
		.z = vector.x * matrix[2][0] + vector.y * matrix[2][1] + vector.z * matrix[2][2] + vector.w * matrix[2][3],
		.w = vector.x * matrix[3][0] + vector.y * matrix[3][1] + vector.z * matrix[3][2] + vector.w * matrix[3][3]
	};
}

vec3f_t vec3f_normalize(const vec3f_t vector) {
	const float magnitude = fabsf(sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z));
	if (!magnitude) // zero vector
		return vector;
	return (vec3f_t){
		vector.x / magnitude,
		vector.y / magnitude,
		vector.z / magnitude
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


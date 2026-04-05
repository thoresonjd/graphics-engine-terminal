/**
 * @file linalg.h
 * @brief Linear Algebra: Vectors & Matrices
 * @author Justin Thoreson
 */

#pragma once
#ifndef LINALG_H
#define LINALG_H

#include <stdint.h>

/**
 * @brief A three-element vector of floats.
 *
 * Can be used to point to indices in a vertex array that form a triangle.
 */
typedef struct {
	uint64_t x, y, z;
} vec3u_t;

/**
 * @brief A three-element vector of floats.
 */
typedef struct {
	float x, y, z;
} vec3f_t;

/**
 * @brief A four-element vector of floats.
 *
 * Typically used for homogeneous coordinates.
 */
typedef struct {
	float x, y, z, w;
} vec4f_t;

/**
 * @brief A four-by-four element matrix of floats.
 */
typedef float mat4f_t[4][4];

/**
 * @brief The identity matrix for a four-by-four matrix of floats.
 */
#define FMAT4_IDENTITY { \
	{ 1.0f, 0.0f, 0.0f, 0.0f }, \
	{ 0.0f, 1.0f, 0.0f, 0.0f }, \
	{ 0.0f, 0.0f, 1.0f, 0.0f }, \
	{ 0.0f, 0.0f, 0.0f, 1.0f }  \
}

/**
 * @brief Multiply two four-by-four matrices together.
 *
 * A naive matrix multiplication approach is sufficient.
 */
void mat4f_multiply(
	mat4f_t out_matrix,
	const mat4f_t matrix_a,
	const mat4f_t matrix_b
);

/**
 * @brief Apply a scalar to a four-by-four matrix of floats.
 *
 * A four-by-four matrix M is scaled with a three-by-three vector V by
 * multiplying each element of V along the diagonal of M.
 *
 * [ m11*v1 m12    m13    m14 ]
 * [ m21    m22*v2 m23    m24 ]
 * [ m31    m32    m33*v3 m34 ]
 * [ m41    m42    m43    m44 ]
 *
 * @param[out] out_matrix The output matrix.
 * @param[in] matrix The input matrix to scale.
 * @param[in] vector The vector whose components are scalars.
 */
void mat4f_scale(
	mat4f_t out_matrix,
	const mat4f_t matrix,
	const vec3f_t vector
);

/**
 * @brief Apply a rotation to a four-by-four matrix of floats.
 * @param[out] out_matrix The output matrix.
 * @param[in] matrix The input matrix to rotate.
 * @param[in] axis The vector representing an axis to rotate around.
 * @param[in] angle The angle in which to rotate.
 * @see https://en.wikipedia.org/wiki/Rotation_matrix#Basic_3D_rotations (Last accessed: 10 March 2026)
 * @see https://learnopengl.com/Getting-started/Transformations (Last accessed: 10 March 2026)
 */
void mat4f_rotate(
	mat4f_t out_matrix,
	const mat4f_t matrix,
	const vec3f_t axis,
	const float angle
);

/**
 * @brief Apply a translation to a four-by-four matrix of floats.
 *
 * A four-by-four matrix M is translated with a three-by-three vector V by
 * adding each element of V along the fourth column of M
 *
 * [ m11 m12 m13 m14+v1 ]
 * [ m21 m22 m23 m24+v2 ]
 * [ m31 m32 m33 m34+v3 ]
 * [ m41 m42 m43 m44    ]
 *
 * @param[out] out_matrix The output matrix.
 * @param[in] matrix The input matrix to translate.
 * @param[in] vector The vector whose components are translators.
 */
void mat4f_translate(
	mat4f_t out_matrix,
	const mat4f_t matrix,
	const vec3f_t vector
);

/**
 * @brief Create a perspective projection matrix.
 * @param[out] out_matrix The output matrix.
 * @param[in] x_fov The field of view along the x-axis (horizontal).
 * @param[in] aspect_ratio The aspect ratio of the viewport.
 * @param[in] z_near The coordinate of the near plane.
 * @param[in] z_far The coordinate of the far plane.
 * @see https://www.songho.ca/opengl/gl_projectionmatrix.html#perspective (Last accessed: 16 March 2026)
 */
void mat4f_perspective(
	mat4f_t out_matrix,
	const float x_fov,
	const float aspect_ratio,
	const float z_near,
	const float z_far
);

/**
 * @brief Create a look at matrix.
 * @param[out] out_matrix The output matrix.
 * @param[in] eye The position of the source.
 * @param[in] target The center of view; the view target.
 * @param[in] world_up Axis pointing upward in world space.
 */
void mat4f_look_at(
	mat4f_t out_matrix,
	const vec3f_t eye,
	const vec3f_t target,
	const vec3f_t world_up
);

/**
 * @brief Multiply a four-element vector of floats with a four-by-four
 *        matrix of floats.
 *
 * To multiply a vector with a matrix, the vector must have the same number of
 * elements as the number of columns within the matrix. Thus, if the vector
 * represents a three-dimensional coordinate in space, the fourth element is
 * the additional coordinate w that makes the coordinates homogeneous.
 *
 * @param[in] vector A four-element vector of floats.
 * @param[in] matrix A four-by-four matrix of floats.
 * @return The resulting multiplied vector.
 */
vec4f_t vec4f_mat4f_multiply(const vec4f_t vector, const mat4f_t matrix);

/**
 * @brief Add two four-element floating point vectors together.
 * @param[in] vector_a The first vector.
 * @param[in] vector_b The second vector.
 * @return The sum of both vectors.
 */
vec4f_t vec4f_add(const vec4f_t vector_a, const vec4f_t vector_b);

/**
 * @brief Subtract two four-element floating point vectors.
 * @param[in] vector_a The first vector.
 * @param[in] vector_b The second vector.
 * @return The difference of both vectors.
 */
vec4f_t vec4f_subtract(const vec4f_t vector_a, const vec4f_t vector_b);

/**
 * @brief Divide a four-element vector of floats by a scalar value.
 * @param[in] vector The vector.
 * @param[in] scalar The scalar.
 * @return The divided vector.
 */
vec4f_t vec4f_float_divide(const vec4f_t vector, const float scalar);

/**
 * @brief Add two three-element floating point vectors together.
 * @param[in] vector_a The first vector.
 * @param[in] vector_b The second vector.
 * @return The sum of both vectors.
 */
vec3f_t vec3f_add(const vec3f_t vector_a, const vec3f_t vector_b);

/**
 * @brief Subtract two three-element floating point vectors.
 * @param[in] vector_a The first vector.
 * @param[in] vector_b The second vector.
 * @return The difference of both vectors.
 */
vec3f_t vec3f_subtract(const vec3f_t vector_a, const vec3f_t vector_b);

/**
 * @brief Multiply a three-element vector of floats by a scalar value.
 * @param[in] vector The vector.
 * @param[in] scalar The scalar.
 * @return The multiplied vector.
 */
vec3f_t vec3f_float_multiply(const vec3f_t vector, const float scalar);

/**
 * @brief Divide a three-element vector of floats by a scalar value.
 * @param[in] vector The vector.
 * @param[in] scalar The scalar.
 * @return The divided vector.
 */
vec3f_t vec3f_float_divide(const vec3f_t vector, const float scalar);

/**
 * @brief Calculate the magnitude of a vector.
 * @param[in] vector The vector to calculate the magnitude of.
 * @return The vector's magnitude.
 */
float vec3f_magnitude(const vec3f_t vector);

/**
 * @brief Normalize a three-element vector of floats.
 * 
 * A vector is normalized by dividing each element by the vector's magnitude.
 *
 * magnitude = abs(sqrt(x^2 + y^2 + z^2))
 * normalized = { x / magnitude, y / magnitude, z / magnitude }
 *
 * @param[in] vector The vector to normalize.
 * @return The normalized vector.
 */
vec3f_t vec3f_normalize(const vec3f_t vector);

/**
 * @brief Compute the dot product (scalar product) of two vectors.
 * @param[in] vector_a The first vector.
 * @param[in] vector_b The second vector.
 * @return The computed dot product.
 */
float vec3f_dot(const vec3f_t vector_a, const vec3f_t vector_b);

/**
 * @brief Compute the cross product of two vectors.
 * @param[in] vector_a The first vector.
 * @param[in] vector_b The second vector.
 * @return The vector computed as the cross product.
 */
vec3f_t vec3f_cross(const vec3f_t vector_a, const vec3f_t vector_b);

#endif // LINALG_H


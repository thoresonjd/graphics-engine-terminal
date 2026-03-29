/**
 * @file render.h
 * @brief Render objects to the screen.
 * @author Justin Thoreson
 */

#pragma once
#ifndef RENDER_H
#define RENDER_H

#include <linalg.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initialize the rendering window/screen.
 * @param[in] width The screen width.
 * @param[in] height The screen height.
 * @return True if initialization was successful, false otherwise.
 */
bool screen_init(const uint8_t width, const uint8_t height);

/**
 * @brief Draw a wireframe from vertices given indices.
 *
 * Each element in the vertices array represents a point in three-dimensional
 * space. Vertices are assumed to conform to homogeneous coordinates (having
 * four elements, the fourth coordinate being w) in clip space when provided
 * as input. Three vertices together form a triangle, the lowest-order polygon
 * to draw.
 *
 * Each element of the indices array represents a vertex triplet, which forms
 * a triangle. Each component of the triplet is an index corresponding to a
 * vertex in the vertices array.
 *
 * @param[in] vertices An array of vertices in clip space.
 * @param[in] num_vertices The number of vertices in the vertices array.
 * @param[in] indices An array of indices.
 * @param[in] num_indices The number of indices in the indices array.
 */
void draw_wireframe(
	const vec4f_t vertices[],
	const uint16_t num_vertices,
	const vec3u_t indices[],
	const uint16_t num_indices
); 

#endif // RENDER_H

/**
 * @file window.h
 * @brief Window: Draw and Render Objects
 * @author Justin Thoreson
 */

#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <linalg.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief The window type.
 */
typedef struct window_t window_t;

/**
 * @brief Initialize the rendering window/screen.
 * @param[in] width The window width.
 * @param[in] height The window height.
 * @return The initialized window if successful, NULL otherwise.
 */
window_t* window_init(const uint8_t width, const uint8_t height);

/**
 * @brief Deallocate window resources, including framebuffer data.
 * @param[in,out] The window to teardown.
 */
void window_teardown(window_t* window);

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
 * Drawing is done directly to the framebuffer. The render() function must be
 * called in order for the framebuffer data to be printed/displayed.
 *
 * @param[in] window The window to draw to.
 * @param[in] vertices An array of vertices in clip space.
 * @param[in] num_vertices The number of vertices in the vertices array.
 * @param[in] indices An array of indices.
 * @param[in] num_indices The number of indices in the indices array.
 * @return True if drawing was successful, false otherwise.
 */
bool window_draw_wireframe(
	const window_t* const window,
	const vec4f_t vertices[],
	const uint16_t num_vertices,
	const vec3u_t indices[],
	const uint16_t num_indices
); 

/**
 * @brief Render framebuffer data within a window.
 * @param[in] window The window to render.
 */
void window_render(const window_t* const window);

/**
 * @brief Determine if the window is open.
 * @param[in] window The window.
 * @return True if the window is open, false otherwise.
 */
bool window_is_open(const window_t* const window);

/**
 * @brief Close the window.
 * @param[in,out] window The window to close.
 */
void window_set_close(window_t* const window);

#endif // WINDOW_H

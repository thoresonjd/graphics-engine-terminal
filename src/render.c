/**
 * @file render.c
 * @brief Render objects to the screen.
 * @author Justin Thoreson
 */

#include <render.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief The ASCII character to print when rendering objects.
 */
static const char PIXEL = '$';

/**
 * @brief The width of the screen.
 *
 * Must be initialized via screen_init.
 */
static uint8_t screen_width = 0;

/**
 * @brief The height of the screen.
 *
 * Must be initialized via screen_init.
 */
static uint8_t screen_height = 0;

/**
 * @brief Determine if a pair of vertices forming a line are contained within
 *        the view frustum (clip space).
 *
 * The current clipping implementation here is naive, as it excludes entire
 * lines from being rendered, even if partially within the view frustum, if
 * either or both vertices forming the line are outside the frustum. This
 * naive implementation is only temporary; vertex_clip is the current
 * workaround, though vertex_clip is still called for all pixels, even those
 * that lie outside the view frustum.
 *
 * @param[in] vertex_a A vertex.
 * @param[in] vertex_b Another vertex.
 * @return True if the line formed by the vertices is entirely within the
 *         view frustum, false otherwise.
 */
static bool line_clip(const vec4f_t vertex_a, const vec4f_t vertex_b);

/**
 * @brief Determine if a vertex lies within the view frustum (clip space).
 *
 * The current clipping implementation here introduces computational overhead.
 * Each vertex, and each pixel interpolated between two vertices, is processed
 * by this function, including pixels/pixel ranges that are entirely outside the
 * view frustum. Thus, and object entirely outside the frustum will still
 * undergoes the drawing process. Instead, in future, this function, along with
 * line_clip, should merge and expand to prevent the drawing process from
 * occuring for excess pixels entirely outside of view.
 *
 * @param[in] vertex A vertex.
 * @return True if the vertex is within the view frustum, false otherwise.
 */
static bool vertex_clip(const vec4f_t vertex);

/**
 * @brief Convert a vertex in clip space to normalized device coordinates via
 *        perspective divide. Perspective divide is achieved in this case by
 *        dividing x, y, and z coordinates by w. 
 * @param[in] vertex A vertex to convert to normalized device coordinates.
 * @return The converted vector in normalized device coordinates.
 */
static vec4f_t vertex_ndc(const vec4f_t vertex);

/**
 * @brief Convert a vertex's coordinates to screen-space.
 *
 * The screen's coordinate system puts the origin (0,0) in the top left.
 * The x-axis grows toward the right, and the y-axis grows downward.
 * 
 * 0,0  x-axis
 *    +----------> width
 *  y |
 *  | |
 *  a |
 *  x |
 *  i |
 *  s |
 *    V
 *   height
 *
 * Clip-space coordinates are, however, within the range of -1 to 1 for both
 * the x-axis and the y-axis, with the origin at the center.
 *           1
 *
 *    y-axis ^
 *           |
 *           |
 *           |
 * -1 <------+------> 1 x-axis
 *           |\
 *           | 0,0
 *           |
 *           V
 *
 *          -1
 *
 * Thus, to go from clip-space to screen-space:
 * 1) Add 1 to each coorinate to bring the range up to 0 to 2
 * 2) Divide by 2 to normalize the coordinate to the range of 0 to 1
 * 3) If a screen-space axis grows in the opposite direction of the
 *    corresponding clip-space axis, take the result of the second step and
 *    subtract it from 1 (y-axis grows down, so subtract from 1).
 * 4) Multiply by the dimension of the corresponding axis (width or height)
 *
 * @param[in] vertex A vertex to convert.
 * @return The converted vertex in screen-space.
 */
static vec4f_t vertex_screen(const vec4f_t vertex);

/**
 * @brief Draw a line between two vertices.
 * @param[in] vertex_a The first vertex.
 * @param[in] vertex_b The second vertex.
 */
static void draw_line(const vec4f_t vertex_a, const vec4f_t vertex_b);

/**
 * @brief Draw a triangle from three vertices.
 * @param[in] vertex_a The first vertex.
 * @param[in] vertex_b The second vertex.
 * @param[in] vertex_c The third vertex.
 */
static void draw_triangle(
	const vec4f_t vertex_a,
	const vec4f_t vertex_b,
	const vec4f_t vertex_c
);

bool screen_init(const uint8_t width, const uint8_t height) {
	if (width <= 0 || height <= 0)
		return false;
	screen_width = width;
	screen_height = height;
	return true;
}

void draw_wireframe(
	const vec4f_t vertices[],
	const uint16_t num_vertices,
	const vec3u_t indices[],
	const uint16_t num_indices
) {
	for (uint16_t i = 0; i < num_indices; i++) {
		if (indices[i].x > num_vertices ||
			indices[i].y > num_vertices ||
			indices[i].z > num_vertices)
			return; // Invalid
		vec4f_t vertex_a = vertices[indices[i].x];
		vec4f_t vertex_b = vertices[indices[i].y];
		vec4f_t vertex_c = vertices[indices[i].z];
		draw_triangle(vertex_a, vertex_b, vertex_c);
	}
}

static bool line_clip(const vec4f_t vertex_a, const vec4f_t vertex_b) {
    if ((vertex_a.x < -vertex_a.w && vertex_b.x < -vertex_b.w) ||
        (vertex_a.x >  vertex_a.w && vertex_b.x >  vertex_b.w) ||
        (vertex_a.y < -vertex_a.w && vertex_b.y < -vertex_b.w) ||
        (vertex_a.y >  vertex_a.w && vertex_b.y >  vertex_b.w) ||
        (vertex_a.z < -vertex_a.w && vertex_b.z < -vertex_b.w) ||
        (vertex_a.z >  vertex_a.w && vertex_b.z >  vertex_b.w))
        return false;
    // TODO: compute partially clipped intersections (see draw_line definition)
    return true;
}

static bool vertex_clip(const vec4f_t vertex) {
    if (vertex.x < -vertex.w || vertex.x >  vertex.w ||
        vertex.y < -vertex.w || vertex.y >  vertex.w ||
        vertex.z < -vertex.w || vertex.z >  vertex.w)
		return false;
	return true;
}

static vec4f_t vertex_ndc(const vec4f_t vertex) {
	return (vec4f_t){
		.x = vertex.x / vertex.w,
		.y = vertex.y / vertex.w,
		.z = vertex.z / vertex.w,
		.w = vertex.w
	};
}

static vec4f_t vertex_screen(const vec4f_t vertex) {
	return (vec4f_t){
		.x = ((vertex.x + 1) / 2) * (float)screen_width,
		.y = (1 - (vertex.y + 1) / 2) * (float)screen_height,
		.z = vertex.z,
		.w = vertex.w
	};
}

static void draw_line(const vec4f_t vertex_a, const vec4f_t vertex_b) {
	/*
     * TODO:
     * Currently, line_clip will prevent a line from being rendered in its
     * entirety if either or both vertices are outside the view frustum.
     * However, vertex_clip currently determines if a single interpolated pixel
     * must be excluded or not, which is a better visual aproach. However, this
     * still means each interpolated pixel must be processed even if the entire
     * line is outside the frustum. In future, lines (and, eventually, full
     * faces) should only have the pixels within the frustum processed by the
     * drawing function to reduce computational overhead.
     */
	if (!line_clip(vertex_a, vertex_b))
		(void)0;//return;
	const uint8_t granularity = 100; // TODO: calculate resolution
	const vec4f_t diff = {
		.x = vertex_b.x - vertex_a.x, 
		.y = vertex_b.y - vertex_a.y,
		.z = vertex_b.z - vertex_a.z,
		.w = vertex_b.w - vertex_a.w
	};
	const vec4f_t increment = {
		.x = diff.x / (float)granularity,
		.y = diff.y / (float)granularity,
		.z = diff.z / (float)granularity,
		.w = diff.w / (float)granularity
	};
	vec4f_t between = vertex_a;
	for (uint8_t i = 0; i <= granularity; i++) {
		if (!vertex_clip(between))
			continue;
		const vec4f_t screen_between = vertex_screen(vertex_ndc(between));
		// TODO: write to a framebuffer instead of printing directly
		if (screen_between.x >= 0.0f &&
			screen_between.x < screen_width &&
			screen_between.y >= 0.0f &&
			screen_between.y < screen_height)
			printf("\x1b[%d;%dH%c", (uint8_t)screen_between.y + 1, (uint8_t)screen_between.x + 1, PIXEL);
		between.x += increment.x;
		between.y += increment.y;
		between.z += increment.z;
		between.w += increment.w;
	}
	fflush(stdout);
}

static void draw_triangle(
	const vec4f_t vertex_a,
	const vec4f_t vertex_b,
	const vec4f_t vertex_c
) {
	draw_line(vertex_a, vertex_b);
	draw_line(vertex_b, vertex_c);
	draw_line(vertex_c, vertex_a);
}


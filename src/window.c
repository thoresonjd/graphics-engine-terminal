/**
 * @file window.c
 * @brief Window: Draw and Render Objects
 * @author Justin Thoreson
 */

#include <window.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief The ASCII character to print when rendering objects.
 */
static const char PIXEL = '$';

/**
 * @brief The window type.
 *
 * Must be initialized with window_init().
 * Must be torn down with window_teardown().
 */
typedef struct window_t {
	/**
	 * @brief The buffer containing pixels to render.
	 */
	char* framebuffer;

	/**
	 * @brief The width of the window.
	 */
	uint8_t width;

	/**
	 * @brief The height of the window.
	 */
	uint8_t height;
} window_t;

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
 * @param[in] window The window whose screen dimensions are used to calculate
 *            the screen coordinates.
 * @param[in] vertex A vertex to convert.
 * @return The converted vertex in screen-space.
 */
static vec4f_t window_vertex_screen(
	const window_t* const window,
	const vec4f_t vertex
);

/**
 * @brief Convert screen coordinates to an index in the framebuffer.
 * @param[in] window The window to get the framebuffer index of.
 * @param[in] x The x coordinate (column).
 * @param[in] y The y coordinate (row).
 * @param[out] index The index to compute.
 * @return True if x and y compute a valid index, false otherwise.
 */
static bool window_framebuffer_index(
	const window_t* const window,
	const uint8_t x,
	const uint8_t y,
	uint16_t* const index
);

/**
 * @brief Write a pixel to the framebuffer
 * @param[in] window The window to draw to.
 * @param[in] screen_pixel Pixel coordinates in screen space.
 * @return True if writing to the framebuffer is successful, false otherwise.
 */
static bool window_write_framebuffer(
	const window_t* const window,
	const vec4f_t screen_pixel
);

/**
 * @brief Draw a line between two vertices.
 * @param[in] window The window to draw to.
 * @param[in] vertex_a The first vertex.
 * @param[in] vertex_b The second vertex.
 * @return True if drawing was successful, false otherwise.
 */
static bool window_draw_line(
	const window_t* const window,
	const vec4f_t vertex_a,
	const vec4f_t vertex_b
);

/**
 * @brief Draw a triangle from three vertices.
 * @param[in] window The window to draw to.
 * @param[in] vertex_a The first vertex.
 * @param[in] vertex_b The second vertex.
 * @param[in] vertex_c The third vertex.
 * @return True if drawing was successful, false otherwise.
 */
static bool window_draw_triangle(
	const window_t* const window,
	const vec4f_t vertex_a,
	const vec4f_t vertex_b,
	const vec4f_t vertex_c
);

static void window_framebuffer_clear(const window_t* const window) {
	const uint16_t window_size = window->width * window->height;
	memset(window->framebuffer, ' ', window_size);
}

window_t* window_init(const uint8_t width, const uint8_t height) {
	if (width <= 0 || height <= 0)
		return NULL;
	window_t* window = (window_t*)calloc(1, sizeof(window_t));
	if (!window)
		return NULL;
	window->width = width;
	window->height = height;
	window->framebuffer = (char*)malloc(width * height);
	if (!window->framebuffer) {
		free(window);
		return NULL;
	}
	window_framebuffer_clear(window);
	return window;
}

void window_teardown(window_t* window) {
	free(window->framebuffer);
	window->framebuffer = NULL;
	free(window);
	window = NULL;
}

bool window_draw_wireframe(
	const window_t* const window,
	const vec4f_t vertices[],
	const uint16_t num_vertices,
	const vec3u_t indices[],
	const uint16_t num_indices
) {
	/*
	 * TODO:
	 * Clearing the framebuffer when drawing each wireframe prevents multiple
	 * wireframes from being drawn, as subsequent calls will override previous
	 * ones. This is only temporary, as programs currently running only render
	 * one wireframe. Eventually, a double buffer and a depth buffer will be
	 * implemented to assist in solving this.
	 */
	window_framebuffer_clear(window);
	for (uint16_t i = 0; i < num_indices; i++) {
		if (indices[i].x > num_vertices ||
			indices[i].y > num_vertices ||
			indices[i].z > num_vertices)
			return false;
		vec4f_t vertex_a = vertices[indices[i].x];
		vec4f_t vertex_b = vertices[indices[i].y];
		vec4f_t vertex_c = vertices[indices[i].z];
		if (!window_draw_triangle(window, vertex_a, vertex_b, vertex_c))
			return false;
	}
	return true;
}

void window_render(const window_t* const window) {
	const uint16_t window_size = window->width * window->height;
	for (uint16_t i = 0; i < window_size; i++) {
		putchar(window->framebuffer[i]);
		if (i % window->width == window->width - 1)
			putchar('\n');
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

static vec4f_t window_vertex_screen(
	const window_t* const window,
	const vec4f_t vertex
) {
	return (vec4f_t){
		.x = ((vertex.x + 1) / 2) * (float)window->width,
		.y = (1 - (vertex.y + 1) / 2) * (float)window->height,
		.z = vertex.z,
		.w = vertex.w
	};
}

static bool window_framebuffer_index(
	const window_t* const window,
	const uint8_t x,
	const uint8_t y,
	uint16_t* const index
) {
	if (x > window->width || y > window->height)
		return false;
	*index = window->width * y + x;
	return true;
}

static bool window_write_framebuffer(
	const window_t* const window,
	const vec4f_t screen_pixel
) {
	if (screen_pixel.x < 0.0f || screen_pixel.x > window->width ||
		screen_pixel.y < 0.0f || screen_pixel.y > window->height)
		return false;
	uint16_t index;
	if (!window_framebuffer_index(window, screen_pixel.x, screen_pixel.y, &index))
		return false;
	window->framebuffer[index] = PIXEL;
	return true;
}

static bool window_draw_line(
	const window_t* const window,
	const vec4f_t vertex_a,
	const vec4f_t vertex_b
) {
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
		const vec4f_t screen_between = window_vertex_screen(window, vertex_ndc(between));
		if (!window_write_framebuffer(window, screen_between))
			return false;
		between.x += increment.x;
		between.y += increment.y;
		between.z += increment.z;
		between.w += increment.w;
	}
	return true;
}

static bool window_draw_triangle(
	const window_t* const window,
	const vec4f_t vertex_a,
	const vec4f_t vertex_b,
	const vec4f_t vertex_c
) {
	return
		window_draw_line(window, vertex_a, vertex_b) &&
		window_draw_line(window, vertex_b, vertex_c) &&
		window_draw_line(window, vertex_c, vertex_a);
}


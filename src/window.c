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

	/**
	 * @brief Determine if the window is open.
	 */
	bool is_open;
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
 * occurring for excess pixels entirely outside of view.
 *
 * @param[in] vertex A vertex.
 * @return True if the vertex is within the view frustum, false otherwise.
 */
static bool vertex_clip(const vec4f_t vertex);

/**
 * @brief Convert a vertex in clip space to normalized device coordinates via
 *        perspective divide. Perspective divide is achieved in this case by
 *        dividing x, y, and z coordinates by w. 
 * @param[out] out_vertex The converted output vertex.
 * @param[in] vertex A vertex to convert to normalized device coordinates.
 * @return True if conversion was successful, false otherwise.
 */
static bool vertex_ndc(vec4f_t* const out_vertex, const vec4f_t vertex);

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
 * 1) Add 1 to each coordinate to bring the range up to 0 to 2
 * 2) Divide by 2 to normalize the coordinate to the range of 0 to 1
 * 3) If a screen-space axis grows in the opposite direction of the
 *    corresponding clip-space axis, take the result of the second step and
 *    subtract it from 1 (y-axis grows down, so subtract from 1).
 * 4) Multiply by the dimension of the corresponding axis (width or height)
 *
 * @param[in] window The window whose screen dimensions are used to calculate
 *            the screen coordinates.
 * @param[out] out_vertex The converted output vertex.
 * @param[in] vertex A vertex to convert.
 * @return True if conversion was successful, false otherwise.
 */
static bool window_vertex_screen(
	const window_t* const window,
	vec4f_t* const out_vertex,
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
 * @brief Clear the window's framebuffer.
 * @param[in,out] window The window whose framebuffer must be cleared.
 * @return True if clearing was successful, false otherwise.
 */
static bool window_framebuffer_clear(window_t* const window);

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
	window->is_open = true;
	if (!window_framebuffer_clear(window))
		return NULL;
	return window;
}

bool window_teardown(window_t* window) {
	if (!window)
		return false;
	if (!window->framebuffer)
		return false;
	free(window->framebuffer);
	window->framebuffer = NULL;
	free(window);
	window = NULL;
	return true;
}

bool window_draw_wireframe(
	window_t* const window,
	const vec4f_t vertices[],
	const uint16_t num_vertices,
	const vec3u_t indices[],
	const uint16_t num_indices
) {
	if (!window)
		return false;
	/*
	 * TODO:
	 * Clearing the framebuffer when drawing each wireframe prevents multiple
	 * wireframes from being drawn, as subsequent calls will override previous
	 * ones. This is only temporary, as programs currently running only render
	 * one wireframe. Eventually, a double buffer and a depth buffer will be
	 * implemented to assist in solving this.
	 */
	if (!window_framebuffer_clear(window))
		return false;
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

bool window_render(const window_t* const window) {
	if (!window)
		return false;
	const uint16_t window_size = window->width * window->height;
	for (uint16_t i = 0; i < window_size; i++) {
		putchar(window->framebuffer[i]);
		if (i % window->width == window->width - 1)
			putchar('\n');
	}
	return true;
}

bool window_is_open(const window_t* const window) {
	if (!window)
		return false;
	return window->is_open;
}

bool window_set_close(window_t* const window) {
	if (!window)
		return false;
	window->is_open = false;
	return true;
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

static bool vertex_ndc(vec4f_t* const out_vertex, const vec4f_t vertex) {
	if (vertex.w == 0.0f)
		return false;
	*out_vertex = (vec4f_t){
		.x = vertex.x / vertex.w,
		.y = vertex.y / vertex.w,
		.z = vertex.z / vertex.w,
		.w = vertex.w
	};
	return true;
}

static bool window_vertex_screen(
	const window_t* const window,
	vec4f_t* const out_vertex,
	const vec4f_t vertex
) {
	if (!window)
		return false;
	*out_vertex = (vec4f_t){
		.x = ((vertex.x + 1.0f) / 2.0f) * (float)window->width,
		.y = (1.0f - (vertex.y + 1.0f) / 2.0f) * (float)window->height,
		.z = vertex.z,
		.w = vertex.w
	};
	return true;
}

static bool window_framebuffer_index(
	const window_t* const window,
	const uint8_t x,
	const uint8_t y,
	uint16_t* const index
) {
	if (!window || x > window->width || y > window->height)
		return false;
	*index = window->width * y + x;
	return true;
}

static bool window_framebuffer_clear(window_t* const window) {
	if (!window)
		return false;
	const uint16_t window_size = window->width * window->height;
	memset(window->framebuffer, ' ', window_size);
	return true;
}

static bool window_write_framebuffer(
	const window_t* const window,
	const vec4f_t screen_pixel
) {
	if (!window ||
		screen_pixel.x < 0.0f || screen_pixel.x > window->width ||
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
	if (!window)
		return false;
	/*
     * TODO:
     * Currently, line_clip will prevent a line from being rendered in its
     * entirety if either or both vertices are outside the view frustum.
     * However, vertex_clip currently determines if a single interpolated pixel
     * must be excluded or not, which is a better visual approach. However, this
     * still means each interpolated pixel must be processed even if the entire
     * line is outside the frustum. In future, lines (and, eventually, full
     * faces) should only have the pixels within the frustum processed by the
     * drawing function to reduce computational overhead.
     */
	if (!line_clip(vertex_a, vertex_b))
		(void)0;
	const uint8_t granularity = 100; // TODO: calculate resolution
	const vec4f_t diff = vec4f_subtract(vertex_b, vertex_a);
	const vec4f_t increment = vec4f_float_divide(diff, granularity);
	vec4f_t between = vertex_a;
	for (uint8_t i = 0; i <= granularity; i++) {
		if (!vertex_clip(between))
			continue;
		vec4f_t ndc_between;
		vec4f_t screen_between;
		if (!vertex_ndc(&ndc_between, between) ||
			!window_vertex_screen(window, &screen_between, ndc_between) ||
			!window_write_framebuffer(window, screen_between))
			return false;
		between = vec4f_add(between, increment);
	}
	return true;
}

static bool window_draw_triangle(
	const window_t* const window,
	const vec4f_t vertex_a,
	const vec4f_t vertex_b,
	const vec4f_t vertex_c
) {
	if (!window)
		return false;
	return
		window_draw_line(window, vertex_a, vertex_b) &&
		window_draw_line(window, vertex_b, vertex_c) &&
		window_draw_line(window, vertex_c, vertex_a);
}


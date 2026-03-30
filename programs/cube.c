/**
 * @file cube.c
 * @brief Rendering a wireframe of a cube.
 * @author Justin Thoreson
 */

#include <linalg.h>
#include <trig.h>
#include <window.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

/**
 * @brief The width of the screen.
 */
static const uint8_t SCREEN_WIDTH = 100;

/**
 * @brief The height of the screen.
 */
static const uint8_t SCREEN_HEIGHT = 50;

/**
 * @brief The aspect ratio of the screen.
 *
 * Typically calculated as screen width over screen height.
 */
static const float ASPECT_RATIO = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

/**
 * @brief The camera's horizontal field of view.
 */
static const float FIELD_OF_VIEW = 90.0f;

/**
 * @brief Where the near-plane resides along the z-axis.
 */
static const float FRUSTUM_NEAR = 0.1f;

/**
 * @brief Where the far-plane resides along the z-axis.
 */
static const float FRUSTUM_FAR = 10.0f;

/**
 * @brief The number of milliseconds per second.
 */
static const uint16_t MILLIS_PER_SECOND = 1000;

/**
 * @brief The number of nanoseconds per millisecond.
 */
static const uint32_t NANOS_PER_MILLI = 1000000;

/**
 * @brief Vertices of a cube. 
 */
static const vec3f_t cube_vertices[8] = {
	// Near face
	{-1.0f,  1.0f, -1.0f}, // top left
	{ 1.0f,  1.0f, -1.0f}, // top right
	{-1.0f, -1.0f, -1.0f}, // bottom left
	{ 1.0f, -1.0f, -1.0f}, // bottom right
	// Far face
	{-1.0f,  1.0f,  1.0f}, // top left
	{ 1.0f,  1.0f,  1.0f}, // top right
	{-1.0f, -1.0f,  1.0f}, // bottom left
	{ 1.0f, -1.0f,  1.0f}  // bottom right
};

/**
 * @brief Indices of a cube.
 *
 * Each element represents a vertex triplet, which forms a triangle. Each
 * component of the triplet is an index corresponding to a vertex in the
 * vertices array.
 */
static const vec3u_t cube_indices[12] = {
	// Front
	{0, 1, 2},
	{1, 2, 3},
	// Back
	{4, 5, 7},
	{4, 6, 7},
	// Top
	{0, 1, 4},
	{1, 4, 5},
	// Bottom
	{2, 3, 7},
	{2, 6, 7},
	// Left
	{0, 2, 4},
	{2, 4, 6},
	// Right
	{1, 3, 7},
	{1, 5, 7}
};

/**
 * @brief Clear the screen.
 */
static void clear_screen();

/**
 * @brief Reset the cursor to the top-left position.
 */
static void reset_cursor();

/**
 * @brief Temporarily halt execution for a specified number of milliseconds.
 * @param[in] delay The number of milliseconds.
 */
static void millisleep(const uint16_t delay);

int main(void) {
	// timing
	const uint8_t frames_per_second = 60;
	const float delta_time_frame = 1.0f/(float)frames_per_second;
	float delta_time_start = 0.0f;
	// transformations
	float angle = 0.0f;
	const vec3f_t axis = { 1.0f, 0.5f, 0.25f };
	const vec3f_t scale = { 0.75f, 0.75f, 0.75 };
	const vec3f_t translation = { 0.0f, 0.0f, -3.0f };
	// screen dimensions
	window_t* window = window_init(SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!window)
		return 1;
	// render loop
	clear_screen();
	while (1) {
		reset_cursor();
	    angle += 180.0f * delta_time_frame;
	    mat4f_t model = FMAT4_IDENTITY;
		mat4f_scale(model, model, scale);
		mat4f_rotate(model, model, axis, angle_radians(angle));
		mat4f_t view = FMAT4_IDENTITY;
		mat4f_translate(view, view, translation);
		mat4f_t projection;
		mat4f_perspective(projection, angle_radians(FIELD_OF_VIEW), ASPECT_RATIO, FRUSTUM_NEAR, FRUSTUM_FAR);
		// ----- start shader -----
		mat4f_t transformation;
		mat4f_multiply(transformation, projection, view);
		mat4f_multiply(transformation, transformation, model);
		vec4f_t transformed[8];
		for (unsigned int i = 0; i < 8; i++) {
			transformed[i] = (vec4f_t){
				.x = cube_vertices[i].x,
				.y = cube_vertices[i].y,
				.z = cube_vertices[i].z,
				.w = 1.0f
			};
			transformed[i] = vec4f_mat4f_multiply(transformed[i], transformation);
		}
		if (!window_draw_wireframe(window, transformed, 8, cube_indices, 12))
			break;
		window_render(window);
		// ----- end shader -----
	    millisleep(MILLIS_PER_SECOND / frames_per_second);
		delta_time_start += delta_time_frame;
	}
	window_teardown(window);
	return 0;
}

static void clear_screen() {
	printf("\x1b[2J");
}

static void reset_cursor() {
	printf("\x1b[H");
}

static void millisleep(const uint16_t delay) {
	const uint8_t seconds = delay / MILLIS_PER_SECOND;
	const uint32_t nanoseconds = NANOS_PER_MILLI * (delay % MILLIS_PER_SECOND);
	struct timespec req = { seconds, nanoseconds };
	nanosleep(&req, NULL);
}


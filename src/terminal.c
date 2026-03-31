/**
 * @file terminal.c
 * @brief Terminal utilities
 * @author Justin Thoreson
 */

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

/**
 * @brief Retrieve the current terminal settings.
 * @return The parameters of the current terminal.
 */
static struct termios terminal_get();

/**
 * @brief Set the attributes of the current terminal.
 * @param[in] terminal The terminal parameters to set.
 */
static void terminal_set(const struct termios terminal);

/**
 * @brief Set a flag for STDIN.
 * @param[in] flag The flag to set.
 */
static void terminal_stdin_set_flag(int flag);

void terminal_clear_screen() {
	printf("\x1b[2J");
}

void terminal_reset_cursor() {
	printf("\x1b[H");
}

void terminal_set_noncanon() {
	struct termios terminal = terminal_get();
	terminal.c_lflag &= ~(ICANON | ECHO);
	terminal_set(terminal);
}

void terminal_set_canon() {
	struct termios terminal = terminal_get();
	terminal.c_lflag |= ICANON | ECHO;
	terminal_set(terminal);
}

void terminal_set_input_nonblocking() {
	terminal_stdin_set_flag(O_NONBLOCK);
}

void terminal_set_input_blocking() {
	terminal_stdin_set_flag(~O_NONBLOCK);
}

char terminal_get_input_char() {
	char c;
	read(STDIN_FILENO, &c, 1);
	return c;
}

static struct termios terminal_get() {
	struct termios terminal;
	tcgetattr(STDIN_FILENO, &terminal);
	return terminal;
}

static void terminal_set(const struct termios terminal) {
	tcsetattr(STDIN_FILENO, TCSANOW, &terminal);
	fflush(stdout);
}

static void terminal_stdin_set_flag(int flag) {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | flag);
}


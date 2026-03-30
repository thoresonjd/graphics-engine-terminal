/**
 * @file terminal.c
 * @brief Terminal utilities
 * @author Justin Thoreson
 */

#include <stdio.h>

void terminal_clear_screen() {
	printf("\x1b[2J");
}

void terminal_reset_cursor() {
	printf("\x1b[H");
}


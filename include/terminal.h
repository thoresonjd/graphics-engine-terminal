/**
 * @file terminal.h
 * @brief Terminal utilities
 * @author Justin Thoreson
 */

#pragma once
#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * @brief Clear the screen.
 */
void terminal_clear_screen();

/**
 * @brief Reset the cursor to the top-left position.
 */
void terminal_reset_cursor();

/**
 * @brief Enable noncanonical mode/disable canonical mode and disable the
 *        echoing for the given terminal attributes.
 */
void terminal_set_noncanon();

/**
 * @brief Enable canonical mode/disable noncanonical mode and enable the
 *        echoing for the given terminal attributes.
 */
void terminal_set_canon();

/**
 * @brief Set terminal input (STDIN) to nonblocking.
 */
void terminal_set_input_nonblocking();

/**
 * @brief Set terminal input (STDIN) to blocking.
 */
void terminal_set_input_blocking();

/**
 * @brief Get terminal input character (from STDIN).
 */
char terminal_get_input_char();

#endif // TERMINAL_H


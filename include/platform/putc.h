#pragma once

/**
 * @brief Prepare to output characters to the screen.
 * 
 * @return int 
 */
int initialize_screen();

/**
 * @brief Write a single character to the screen.
 * 
 * @param c 
 * @return int 
 */
int putchar(int c);

/**
 * @brief Write a string to the screen.
 * 
 * @param str 
 * @return int 
 */
int puts(const char *str);
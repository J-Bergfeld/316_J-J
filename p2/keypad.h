/**
 * @file    keypad.h
 * @brief   3x4 matrix keypad driver interface.
 *
 * Provides initialization and polling functions for a 3-column by 4-row
 * matrix keypad scanned using GPIO.
 *
 * Functions:
 *   - keypad_init()   : Configure GPIO pins used by the keypad driver
 *   - keypad_getkey() : Return key code or -1 if no key is pressed
 *
 * Return values:
 *   - keypad_getkey() returns:
 *       -1  : no key pressed
 *       0-9 : digit keys (as mapped in keypad.c)
 *       10  : '*' (if mapped that way in keypad.c)
 *       11  : '#' (if mapped that way in keypad.c)
 *       (Chat-gpt formatted header)
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

/**
 * @brief   Initialize GPIO pins for keypad scanning.
 *
 * Configures row pins as input with pull-down resistors and column pins
 * as push-pull outputs.
 */
void keypad_init(void);

/**
 * @brief   Scan the keypad and return the detected key code.
 *
 * @return  Key code (see keypad.c keymap) or -1 if no key is pressed.
 */
int16_t keypad_getkey(void);

#endif /* KEYPAD_H */

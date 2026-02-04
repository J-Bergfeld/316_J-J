/**
 * @file    leds.h
 * @brief   LED control interface for 4-bit display.
 *
 * This module provides functions to initialize and drive four LEDs used
 * as a 4-bit output display.
 *
 * Hardware mapping:
 *   - PC0 : LED bit 0 (LSB)
 *   - PC1 : LED bit 1
 *   - PC2 : LED bit 2
 *   - PA5 : LED bit 3 (MSB / onboard LED)
 *
 * Public functions:
 *   - led_init()         : Configure GPIO pins for LED output
 *   - display_on_leds()  : Display the lower 4 bits of a value on LEDs
 * (chatgpt formatted header)
 */

#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

/**
 * @brief   Initialize GPIO pins used for LED output.
 *
 * Configures PC0, PC1, PC2, and PA5 as push-pull outputs with no
 * internal pull-up or pull-down resistors.
 */
void led_init(void);

/**
 * @brief   Display the lower 4 bits of a value on the LEDs.
 *
 * Bit mapping:
 *   - bit 0 -> PC0
 *   - bit 1 -> PC1
 *   - bit 2 -> PC2
 *   - bit 3 -> PA5
 *
 * @param   count  Signed 16-bit value to display.
 */
void display_on_leds(int16_t count);

#endif /* LEDS_H */

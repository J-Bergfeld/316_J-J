/**
 * @file    main.c
 * @brief   Application entry point for keypad-to-LED display.
 *
 * This file contains the main application loop. The program initializes
 * the HAL, LED GPIO hardware, and keypad GPIO hardware. It then polls the
 * keypad and displays the detected key value on the LEDs.
 *
 * Modules used:
 *   - keypad: Scans a 3x4 matrix keypad and returns an int16_t key code.
 *   - leds  : Drives an LED display and renders an int16_t value.
 *
 * Notes:
 *   - The keypad is polled in the while(1) loop.
 *   - A short delay is inserted after a valid key press to reduce bounce
 *     and repeated detections.
 *
 * Dependencies:
 *   - STM32 HAL initialization (HAL_Init)
 *   - keypad_init(), keypad_getkey()
 *   - led_init(), display_on_leds()
 *
 * @author  Jack Sevigny
 * @date    2026-01-15
 * (chat-gpt formatted header)
 */


#include "main.h"
#include "keypad.h"
#include "leds.h"
#include <stdint.h>

#define KEY_DEBOUNCE_DELAY_MS (200u)  //Key debounce/repeat delay (ms).

int main(void)
{
    int16_t key_code = (int16_t)-1;

    HAL_Init();
    led_init();
    keypad_init();

    while (1) {
        key_code = keypad_getkey();
        if (key_code != (int16_t)-1) {
            display_on_leds(key_code);
            HAL_Delay(KEY_DEBOUNCE_DELAY_MS); // delay
        }
    }
}

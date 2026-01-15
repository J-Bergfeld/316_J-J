#include "keypad.h"
#include "stm32l4xx.h"
#include <stdint.h>

#define NUM_ROWS 4u
#define NUM_COLS 3u

#define ROW_PORT GPIOB
#define COL_PORT GPIOB

#define ROW_PINS ((1u<<0)|(1u<<1)|(1u<<2)|(1u<<3))  // PB0..PB3
#define COL_OFFSET 4u                                // PB4 is first column
#define COL_MASK (((1u << NUM_COLS) - 1u) << COL_OFFSET)
#define COL_CLEAR (~(uint32_t)COL_MASK)

void keypad_init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    /* Rows PB0..PB3: input + pull-down */
    for (unsigned pin = 0u; pin < NUM_ROWS; pin++) {
        ROW_PORT->MODER &= ~(3u << (2u * pin));          // input
        ROW_PORT->PUPDR &= ~(3u << (2u * pin));
        ROW_PORT->PUPDR |=  (2u << (2u * pin));          // pull-down
    }

    /* Cols PB4..PB6: output + push-pull + low speed + no pull + default low */
    for (unsigned i = 0u; i < NUM_COLS; i++) {
        unsigned pin = i + COL_OFFSET;

        COL_PORT->MODER &= ~(3u << (2u * pin));
        COL_PORT->MODER |=  (1u << (2u * pin));          // output

        COL_PORT->OTYPER  &= ~(1u << pin);               // push-pull
        COL_PORT->OSPEEDR &= ~(3u << (2u * pin));        // low speed
        COL_PORT->PUPDR   &= ~(3u << (2u * pin));        // no pull
    }

    COL_PORT->ODR &= COL_CLEAR;                          // columns low
}

int16_t keypad_getkey(void)
{
    static const int16_t keymap[4][3] = {
        {  1,  2,  3 },
        {  4,  5,  6 },
        {  7,  8,  9 },
        { -2,  0, -3 }
    };

    /* Stage 1: detect any key */
    COL_PORT->ODR |= COL_MASK;                           // all columns high

    uint32_t rows = (ROW_PORT->IDR & ROW_PINS);
    if (rows == 0u) {
        COL_PORT->ODR &= COL_CLEAR;                      // all columns low
        return (int16_t)-1;								 // return if no press
    }

    /* Stage 2: identify key */
    for (unsigned c = 0u; c < NUM_COLS; c++) {
        COL_PORT->ODR &= COL_CLEAR;                      // all columns low
        COL_PORT->ODR |= (1u << (c + COL_OFFSET));       // one column high

        rows = (ROW_PORT->IDR & ROW_PINS);
        if (rows != 0u) {
            for (unsigned r = 0u; r < NUM_ROWS; r++) {
                if (rows & (1u << r)) {
                    COL_PORT->ODR &= COL_CLEAR;          // leave low
                    return keymap[r][c];
                }
            }
        }
    }

    COL_PORT->ODR &= COL_CLEAR;
    return (int16_t)-1;
}

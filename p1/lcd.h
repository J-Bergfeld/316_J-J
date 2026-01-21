/*
 * lcd.h
 *
 *  Created on: Jan 19, 2026
 *      Author: jackb
 */

#ifndef LCD_H
#define LCD_H

#include "stm32l4xx_hal.h"
#include <stdint.h>

// Public API
void LCD_init(void);
void LCD_command(uint8_t cmd);
void LCD_write_char(uint8_t ch);
void LCD_write_string(const char *s);
void LCD_set_cursor(uint8_t row, uint8_t col);

#endif

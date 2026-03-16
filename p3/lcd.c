/*
 * lcd.c
 *
 *  Created on: Jan 19, 2026
 *      Author: jackb
 */

#include "lcd.h"

// -------- Pin mapping --------
#define LCD_PORT_DATA      GPIOC
#define LCD_PORT_CTRL      GPIOC

#define LCD_DATA_MASK      (0x00FFu)          // PC0..PC7
#define LCD_RS_PIN         GPIO_PIN_8         // PC8
#define LCD_E_PIN          GPIO_PIN_9         // PC9

// E pulse timing at 1 us
static void lcd_delay_short(void)
{
    // A few microseconds worth of NOPs
    for (volatile int i = 0; i < 200; i++) {
        __NOP();
    }
}

static void lcd_pulse_enable(void)
{
    // E high -> short -> E low
    LCD_PORT_CTRL->BSRR = LCD_E_PIN;          // set
    lcd_delay_short();
    LCD_PORT_CTRL->BSRR = (LCD_E_PIN << 16);  // reset
    lcd_delay_short();
}

static void lcd_write_bus(uint8_t value)
{
    // Put value on PC0 to PC7
    uint32_t odr = LCD_PORT_DATA->ODR;
    odr &= ~LCD_DATA_MASK;
    odr |= (uint32_t)value;
    LCD_PORT_DATA->ODR = odr;
}

static void lcd_write(uint8_t value, uint8_t rs)
{
    // rs=0 command, rs=1 data
    if (rs) LCD_PORT_CTRL->BSRR = LCD_RS_PIN;
    else    LCD_PORT_CTRL->BSRR = (LCD_RS_PIN << 16);

    lcd_write_bus(value);
    lcd_pulse_enable();

    // instruction delay jus to be safe
    HAL_Delay(1);
}

// -------- Public functions --------
void LCD_command(uint8_t cmd)
{
    lcd_write(cmd, 0);
}

void LCD_write_char(uint8_t ch)
{
    lcd_write(ch, 1);
}

void LCD_write_string(const char *s)
{
    while (*s) {
        LCD_write_char((uint8_t)*s++);
    }
}

void LCD_set_cursor(uint8_t row, uint8_t col)
{

    uint8_t addr = (row == 0) ? (0x00 + col) : (0x40 + col);
    LCD_command(0x80 | addr);
}

void LCD_init(void)
{
    // Enable GPIOC clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // PC0 through PC9 as outputs
    // MODER: 01 for output
    // Clear then set for pins 0..9
    GPIOC->MODER &= ~(
        GPIO_MODER_MODE0  | GPIO_MODER_MODE1  | GPIO_MODER_MODE2  | GPIO_MODER_MODE3  |
        GPIO_MODER_MODE4  | GPIO_MODER_MODE5  | GPIO_MODER_MODE6  | GPIO_MODER_MODE7  |
        GPIO_MODER_MODE8  | GPIO_MODER_MODE9
    );
    GPIOC->MODER |= (
        GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 |
        GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0 |
        GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0
    );

    // Push-pull, no pull-ups, medium speed
    GPIOC->OTYPER &= ~(
        GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 |
        GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7 |
        GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9
    );
    GPIOC->PUPDR &= ~(
        GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 |
        GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7 |
        GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9
    );

    // Start with control low
    GPIOC->BSRR = (LCD_RS_PIN << 16);
    GPIOC->BSRR = (LCD_E_PIN  << 16);

    // Wait for LCD power-up
    HAL_Delay(50);

    // Initialization sequence (8-bit, 2-line, 5x8 dots)
    LCD_command(0x38);  // Function set: 8-bit, 2-line
    LCD_command(0x0F);  // Display ON, cursor ON, blink ON
    LCD_command(0x01);  // Clear display
    HAL_Delay(2);       // Clear needs longer so delay that thang accordingly
    LCD_command(0x06);  // Entry mode: increment, no shift
}


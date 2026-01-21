#include "lock.h"
#include "stm32l4xx.h"
#include "lcd.h"
#include "keypad.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define KEY_NONE   ((int16_t)-1)
#define KEY_STAR   ((int16_t)10)   // '*' i think
#define KEY_POUND  ((int16_t)11)   // '#' double check

#define PIN_LEN 4
#define DEBOUNCE_MS 200u

typedef enum {
  LOCK_STATE_LOCKED = 0,
  LOCK_STATE_UNLOCKED,
  LOCK_STATE_SET_NEW_PIN
} lock_state_t;

static lock_state_t state;

static char current_pin[PIN_LEN + 1] = "1234";   // made as the default pin to get in and unlock
static char entry[PIN_LEN + 1];
static uint8_t entry_len;

static void delay_ms(uint32_t ms)
{
  HAL_Delay(ms);
}

static void lcd_clear(void)
{
  // Standard HD44780 clear display command
  LCD_command(0x01);
  delay_ms(2);
}

static void lcd_line0(const char *s)
{
  LCD_set_cursor(0, 0);
  LCD_write_string("                "); // clear line (16 spaces)
  LCD_set_cursor(0, 0);
  LCD_write_string(s);
}

static void lcd_line1(const char *s)
{
  LCD_set_cursor(1, 0);
  LCD_write_string("                ");
  LCD_set_cursor(1, 0);
  LCD_write_string(s);
}

static void set_locked_led(bool locked)
{
  // On NUCLEO-L476RG, onboard LED is PA5.
  // Locked => LED ON, Unlocked => LED OFF
  if (locked) GPIOA->ODR |=  (1u << 5);
  else        GPIOA->ODR &= ~(1u << 5);
}

static void gpio_init_led_pa5(void)
{
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

  // PA5 output
  GPIOA->MODER &= ~(3u << (2u * 5u));
  GPIOA->MODER |=  (1u << (2u * 5u));

  // push-pull, no pull
  GPIOA->OTYPER &= ~(1u << 5);
  GPIOA->PUPDR  &= ~(3u << (2u * 5u));
}

static void reset_entry(void)
{
  memset(entry, 0, sizeof(entry));
  entry_len = 0;
}

static void show_locked_screen(void)
{
  lcd_clear();
  lcd_line0("LOCKED");
  lcd_line1("ENTER KEY:");
}

static void show_unlocked_screen(void)
{
  lcd_clear();
  lcd_line0("UNLOCKED");
  lcd_line1("PRESS KEY TO LOCK");
}

static void show_newpin_screen(void)
{
  lcd_clear();
  lcd_line0("UNLOCKED");
  lcd_line1("NEW PIN:");
}

static void update_entry_on_lcd(void)
{
  // Show: "ENTER KEY:1234" style (or NEW PIN:1234)
  char buf[17];

  if (state == LOCK_STATE_SET_NEW_PIN)
  {
    snprintf(buf, sizeof(buf), "NEW PIN:%s", entry);
    lcd_line1(buf);
  }
  else
  {
    snprintf(buf, sizeof(buf), "ENTER KEY:%s", entry);
    lcd_line1(buf);
  }
}

static bool is_digit_key(int16_t k)
{
  return (k >= 0 && k <= 9);
}

void lock_init(void)
{
  gpio_init_led_pa5();
  set_locked_led(true);

  state = LOCK_STATE_LOCKED;
  reset_entry();
  show_locked_screen();
}

void lock_task(void)
{
  int16_t k = keypad_getkey();
  if (k == KEY_NONE)
    return;

  // Debounce / prevent repeats
  delay_ms(DEBOUNCE_MS);

  if (state == LOCK_STATE_LOCKED)
  {
    if (k == KEY_STAR)
    {
      reset_entry();
      show_locked_screen();
      return;
    }

    if (is_digit_key(k))
    {
      if (entry_len < PIN_LEN)
      {
        entry[entry_len++] = (char)('0' + k);
        entry[entry_len] = '\0';
        update_entry_on_lcd();
      }

      if (entry_len == PIN_LEN)
      {
        if (strncmp(entry, current_pin, PIN_LEN) == 0)
        {
          // Correct pin gives the unlock yippie
          set_locked_led(false);
          state = LOCK_STATE_UNLOCKED;
          reset_entry();
          show_unlocked_screen();
        }
        else
        {
          // Wrong pin keeps it locked an prints message
          lcd_line0("LOCKED");
          lcd_line1("BAD PIN");
          delay_ms(800);
          reset_entry();
          show_locked_screen();
        }
      }
    }

    return;
  }

  if (state == LOCK_STATE_UNLOCKED)
  {
    // While unlocked:
    // - using the # enters pin change mode
    // - if you press another key the lock should relock with the current pin still
    if (k == KEY_POUND)
    {
      state = LOCK_STATE_SET_NEW_PIN;
      reset_entry();
      show_newpin_screen();
      return;
    }
    else
    {
      set_locked_led(true);
      state = LOCK_STATE_LOCKED;
      reset_entry();
      show_locked_screen();
      return;
    }
  }

  // LOCK_STATE_SET_NEW_PIN
  if (k == KEY_STAR)
  {
    reset_entry();
    show_newpin_screen();
    return;
  }

  if (is_digit_key(k))
  {
    if (entry_len < PIN_LEN)
    {
      entry[entry_len++] = (char)('0' + k);
      entry[entry_len] = '\0';
      update_entry_on_lcd();
    }

    if (entry_len == PIN_LEN)
    {
      // Save new pin
      memcpy(current_pin, entry, PIN_LEN);
      current_pin[PIN_LEN] = '\0';

      lcd_line0("UNLOCKED");
      lcd_line1("PIN SET");
      delay_ms(800);

      state = LOCK_STATE_UNLOCKED;
      reset_entry();
      show_unlocked_screen();
    }
  }
}

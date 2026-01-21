#include "main.h"
#include "lcd.h"
#include "keypad.h"
#include "lock.h"

int main(void)
{
  HAL_Init();
//init the perfs
  LCD_init();
  keypad_init();

  // Init the locking application
  lock_init();

  while (1)
  {
    lock_task();
  }
}

#include "main.h"
#include "keypad.h"
#include "leds.h"

int main(void)
{
	HAL_Init();
	led_init();
	keypad_init();

  while (1)
  {
	  int16_t key = keypad_getkey(); //get key pressed
	  if (key != -1) {
		  display_on_leds(key);
	      HAL_Delay(200);   // allowed HAL func
	  }
  }// end main
}


#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>

// Call once at startup (after HAL_Init, LCD_init, keypad_init)
void lock_init(void);

// Call repeatedly in while(1)
void lock_task(void);

#endif

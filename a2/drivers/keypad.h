/*
 * keypad.h
 *
 *  Created on: Jan 12, 2026
 *      Author: Jack and Jack
 */

#ifndef SRC_DRIVERS_KEYPAD_H_
#define SRC_DRIVERS_KEYPAD_H_

int16_t keypad_init(void);	   // Initializes pins
int16_t keypad_getkey(void);   // returns -1 if no key is pressed or the value of the key

#endif /* SRC_DRIVERS_KEYPAD_H_ */

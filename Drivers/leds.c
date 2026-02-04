/**
 * @file    leds.c
 * @brief   LED control interface.
 *
 * This module provides initialization and display functions for a
 * 4-bit LED output implemented using GPIO pins PC0–PC2 and PA5.
 *
 * The LEDs are used to display a signed 16-bit value, typically
 * originating from keypad input or other application logic.
 *
 * Public functions:
 *   - led_init()         : Configure GPIO pins for LED output
 *   - display_on_leds()  : Display a 4-bit value on the LEDs
 *
 * Hardware mapping:
 *   - PC0 : LED bit 0 (LSB)
 *   - PC1 : LED bit 1
 *   - PC2 : LED bit 2
 *   - PA5 : LED bit 3 (MSB / onboard LED)
 *   (chat-gpt formatted header and file layout)
 */
#include "leds.h"
#include "stm32l4xx.h"
#include <stdint.h>


void led_init(void) {


	  // Enable GPIOA and GPIOC clocks
	  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN);

	  // ---- PC0, PC1, PC2 as outputs (01) ----
	  GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2);
	  GPIOC->MODER |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0);

	  GPIOA->MODER &= ~(GPIO_MODER_MODE5);
	  GPIOA->MODER |=  (GPIO_MODER_MODE5_0);
	  GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2); // push-pull
	  GPIOC->PUPDR  &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2); // no pull

	  // ---- PA5 as output (01) ---
	  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5);
	  GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPD5);




}
void display_on_leds(int16_t count) {
	      // Clear PC0..PC2, PA5 then set bits based on input
		  uint16_t u = (uint16_t)count;
	      GPIOC->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
	      GPIOA->ODR &= ~(GPIO_ODR_OD5);
	      if (u == 0) {
	    	  GPIOC->ODR &= ~(GPIO_ODR_OD0);
	    	  GPIOA->ODR &= ~(GPIO_ODR_OD5);
	      }
	      if (u & (1u << 0)) GPIOC->ODR |= GPIO_ODR_OD0;
	      if (u & (1u << 1)) GPIOC->ODR |= GPIO_ODR_OD1;
	      if (u & (1u << 2)) GPIOC->ODR |= GPIO_ODR_OD2;
	      if (u & (1u << 3)) GPIOA->ODR |= GPIO_ODR_OD5;// set pin 5 of PORT A to 1 for onboard led
	  }
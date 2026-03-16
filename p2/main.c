/*
 * main.c
 *
 * Top-level application control.
 * Handles keypad input and maps user actions to waveform
 * generation and timer-driven DAC output.
 */

#include "stm32l4xx_hal.h"
#include <stdint.h>
#include "stm32l476xx.h"
#include "dac.h"
#include "keypad.h"
#include "wavegen.h"
#include "tim2_waveout.h"

/* ---------- Configuration Constants ---------- */



/* ---------- Main ---------- */
#include "stm32l476xx.h"
#include "main.h"

int main(void)
{
    HAL_Init();

    // Enable GPIOA and GPIOC clocks
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN);

    // -------- PA1 (MOSFET output) --------
    GPIOA->MODER &= ~(GPIO_MODER_MODE1);
    GPIOA->MODER |=  (GPIO_MODER_MODE1_0);     // output mode

    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT1);       // push-pull
    GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPD1);      // no pull
    GPIOA->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED1_Pos);

    GPIOA->BRR = GPIO_BRR_BR1;                 // ensure launcher OFF

    // -------- PC13 (Nucleo user button B1) --------
    GPIOC->MODER &= ~(GPIO_MODER_MODE13);      // input mode
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13);      // no pull

    while (1)
    {
        // Button pressed (active LOW)
        if(!(GPIOC->IDR & GPIO_IDR_ID13))
        {
            // Launch
            GPIOA->BSRR = GPIO_BSRR_BS1;       // PA1 HIGH
            HAL_Delay(3000);                   // 3 seconds
            GPIOA->BRR = GPIO_BRR_BR1;         // PA1 LOW

            // wait for button release (debounce)
            while(!(GPIOC->IDR & GPIO_IDR_ID13));
            HAL_Delay(50);
        }
    }
}

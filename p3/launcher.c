#include "stm32l476xx.h"
#include "launcher.h"
#include "main.h"   // for HAL_Delay()

void launcher_init(void)
{
    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Set PA1 to output mode
    GPIOA->MODER &= ~(GPIO_MODER_MODE1);
    GPIOA->MODER |=  (GPIO_MODER_MODE1_0);

    // Push-pull output
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT1);

    // No pull-up / pull-down
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD1);

    // High speed
    GPIOA->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED1_Pos);

    // Ensure launcher is OFF
    GPIOA->BRR = GPIO_BRR_BR1;
}

void launch(void)
{
    // Turn MOSFET ON
    GPIOA->BSRR = GPIO_BSRR_BS1;

    // Keep it on for 3 seconds
    HAL_Delay(3000);

    // Turn MOSFET OFF
    GPIOA->BRR = GPIO_BRR_BR1;
}

#include "main.h"
#include <stdint.h>

static const uint16_t ARR_VAL  = 799u;
static const uint16_t CCR1_VAL = 200u;

int main(void)
{
	// Enable MCO, select MSI (4 MHz source)
	RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));
	// Configure MCO output on PA8
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
	GPIOA->MODER &= ~(GPIO_MODER_MODE8); // alternate function mode
	GPIOA->MODER |= (2 << GPIO_MODER_MODE8_Pos);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT8); // Push-pull output
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD8); // no resistor
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED8); // high speed
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8); // select MCO function

	HAL_Init();
	SystemClock_Config();

	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN);

	GPIOA->MODER   &= ~(GPIO_MODER_MODE5);
	GPIOA->MODER   |=  (GPIO_MODER_MODE5_0);
	GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT5);
	GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD5);
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5);

	GPIOC->MODER &= ~(GPIO_MODER_MODE13);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13);

	__enable_irq();

	NVIC->ISER[0] = (1U << (TIM2_IRQn & 0x1F));

	RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);      // turn on TIM2
	TIM2->DIER    |= (TIM_DIER_CC1IE);          // enable interrupts on channel 1
	TIM2->SR      &= ~(TIM_SR_CC1IF);           // clear interrupt flag

	// 1) RCC enable TIM2 clock (APB1)
	// RCC->APB1ENR1 |= ... TIM2EN ...

	// 2) TIM2 basic upcount config
	TIM2->CR1 = 0;                 // DIR=0 upcount, etc.
	TIM2->PSC = 0;
	TIM2->ARR = ARR_VAL;
	TIM2->CCR1 = CCR1_VAL;
	TIM2->CR1     |=  TIM_CR1_CEN;              // start timer
	// 3) Interrupt enables
	TIM2->DIER = 0;
	TIM2->DIER |= (1u<<0);         // UIE (update interrupt)
	TIM2->DIER |= (1u<<1);         // CC1IE

	// 4) Load registers immediately
	TIM2->EGR = 1u;                // UG

	// 5) Start
	TIM2->CR1 |= 1u;               // CEN

	while (1)
	{
	}


}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~(TIM_SR_CC1IF);
    TIM2->CCR1 += (1U << 19);

    if (GPIOA->ODR & GPIO_PIN_5)
    {
        GPIOA->BRR = GPIO_PIN_5;
    }
    else
    {
        GPIOA->BSRR = GPIO_PIN_5;
    }
}


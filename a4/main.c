/******************************************************************************
 * File: main.c
 *
 * Description:
 * Initializes GPIO and TIM2 to generate a periodic interrupt. The timer
 * interrupt updates a software-controlled GPIO output.
 *
 *****************************************************************************/
#include "main.h"
#include <stdint.h>
#include "stm32l476xx.h"

#define PSC_VAL 0xFA0

static const uint16_t ARR_VAL  = 799u;
static const uint16_t TIM_CLK = SystemCoreClock;
static const uint16_t CCR1_VAL = 199u;


int main(void)
{

	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);

	//gpio config
	GPIOA->MODER   &= ~(GPIO_MODER_MODE1);
	GPIOA->MODER   |=  (GPIO_MODER_MODE1_0);
	GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT1);
	GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD1);
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED1);
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED1_1;

	
	tim2_waveout_init(5000);


	while (1)
	{
	}


}

void tim2_waveout_init(uint16_t wave_freq_hz, uint16_t duty)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // 2) TIM2 basic upcount config
	TIM2->CR1 = 0;
	TIM2->PSC = 0xFA0;           // 1 Hz
	TIM2->CCR1 = CCR1_VAL;
	TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE); //Enable Interupts
	TIM2->SR   &= ~(TIM_SR_CC1IF);
	TIM2->ARR = (TIM_CLK/wave_freq_hz)-1;
	TIM2->CR1 |= TIM_CR1_ARPE;
	TIM2->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= (1U << (TIM2_IRQn & 0x1F));
    __enable_irq();

    TIM2->CR1 |= TIM_CR1_CEN;
}


//ISR
void TIM2_IRQHandler(void)
{

    if (TIM2->SR & TIM_SR_UIF) {
        GPIOA->ODR |= GPIO_PIN_1;
        TIM2->SR &= ~(TIM_SR_UIF);
    } else {
        GPIOA->ODR &= ~GPIO_PIN_1;
        TIM2->SR &= ~(TIM_SR_CC1IF);
    }

}

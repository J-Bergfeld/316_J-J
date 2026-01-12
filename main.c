#include "main.h"

int main(void)
{
  HAL_Init();

  // Enable GPIOA and GPIOC clocks
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN);

  // ---- PC0, PC1, PC2 as outputs (01) ----
  GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2);
  GPIOC->MODER |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0);

  GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2); // push-pull
  GPIOC->PUPDR  &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2); // no pull

  // ---- PA4 as input (00) with pull-up (01) ----
  GPIOA->MODER &= ~(GPIO_MODER_MODE4);
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4);
  GPIOA->PUPDR |=  (GPIO_PUPDR_PUPD4_0);  // pull-up

  uint8_t count = 0;

  while (1)
  {
    // PA4 LOW means "pressed" (wired to GND)
    if ((GPIOA->IDR & GPIO_IDR_ID4) == 0)
    {
      // Clear PC0..PC2 then set bits based on count
      GPIOC->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
      if (count & 0x1) GPIOC->ODR |= GPIO_ODR_OD0;
      if (count & 0x2) GPIOC->ODR |= GPIO_ODR_OD1;
      if (count & 0x4) GPIOC->ODR |= GPIO_ODR_OD2;

      count = (count + 1) & 0x7;
      HAL_Delay(200);   // allowed HAL function
    }
    // else: do nothing / hold current LED pattern
  }
}

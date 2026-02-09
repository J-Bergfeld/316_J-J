#include "dac.h"
#include "stm32l476xx.h"

#define DAC_CS_LOW()   (GPIOA->ODR &= ~(1U << 4))
#define DAC_CS_HIGH()  (GPIOA->ODR |=  (1U << 4))

void DAC_init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;

    GPIOA->MODER &= ~(3U << (2*4));
    GPIOA->MODER |=  (1U << (2*4));
    GPIOA->OTYPER &= ~(1U << 4);
    GPIOA->OSPEEDR |= (3U << (2*4));
    GPIOA->PUPDR &= ~(3U << (2*4));
    DAC_CS_HIGH();

    GPIOC->MODER &= ~(3U << (2*10));
    GPIOC->MODER |=  (2U << (2*10));
    GPIOC->AFR[1] &= ~(0xFU << (4*(10-8)));
    GPIOC->AFR[1] |=  (6U   << (4*(10-8)));

    GPIOC->MODER &= ~(3U << (2*12));
    GPIOC->MODER |=  (2U << (2*12));
    GPIOC->AFR[1] &= ~(0xFU << (4*(12-8)));
    GPIOC->AFR[1] |=  (6U   << (4*(12-8)));

    SPI3->CR1 = 0;
    SPI3->CR1 |= SPI_CR1_MSTR;
    SPI3->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI3->CR1 |= SPI_CR1_BR_0; // fPCLK/4 (safe starting point)

    SPI3->CR2 = 0;
    SPI3->CR2 |= (SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0); // 16-bit

    SPI3->CR1 |= SPI_CR1_SPE;
}

void DAC_write(uint16_t code)
{
    code &= 0x0FFFU; // NEW: force 12-bit so out-of-range math can’t corrupt control bits

    // NEW: build full MCP4921 frame every time (control bits + 12-bit data)
    // 0b0011 = BUF=0, GA=1x, SHDN=1 (active)
    uint16_t frame = (uint16_t)((0x3U << 12) | code);

    DAC_CS_LOW();

    while (!(SPI3->SR & SPI_SR_TXE)) {}
    *((volatile uint16_t*)&SPI3->DR) = frame;
    while (SPI3->SR & SPI_SR_BSY) {}

    DAC_CS_HIGH();
}

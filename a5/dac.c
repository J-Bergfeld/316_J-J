#include "dac.h"
#include "stm32l4xx.h"

// ---- Pin choices ----
// SPI3: PC10=SCK (AF6), PC12=MOSI (AF6)
// CS:   PA4 (GPIO output)

#define CS_PORT GPIOA
#define CS_PIN  4u

static inline void cs_low(void)  { CS_PORT->BRR  = (1u << CS_PIN); }
static inline void cs_high(void) { CS_PORT->BSRR = (1u << CS_PIN); }

// Write one 16-bit frame on SPI3 (blocking)
static void spi3_write16(uint16_t data)
{
  // Wait TXE (transmit buffer empty)
  while ((SPI3->SR & SPI_SR_TXE) == 0) {}

  // 16-bit access to DR
  *(__IO uint16_t *)&SPI3->DR = data;

  // Wait until not busy
  while ((SPI3->SR & SPI_SR_BSY) != 0) {}
}

void DAC_init(void)
{
  // --- Enable clocks ---
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN;
  RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;

  // --- Configure PA4 as GPIO output for CS ---
  // MODER: 01 = output
  GPIOA->MODER &= ~(3u << (2u * CS_PIN));
  GPIOA->MODER |=  (1u << (2u * CS_PIN));
  // push-pull, no pull
  GPIOA->OTYPER &= ~(1u << CS_PIN);
  GPIOA->PUPDR  &= ~(3u << (2u * CS_PIN));
  // start inactive (high)
  cs_high();

  // --- Configure PC10, PC12 as AF6 for SPI3 ---
  // PC10: MODER=10 (AF)
  GPIOC->MODER &= ~(3u << (2u * 10u));
  GPIOC->MODER |=  (2u << (2u * 10u));
  // PC12: MODER=10 (AF)
  GPIOC->MODER &= ~(3u << (2u * 12u));
  GPIOC->MODER |=  (2u << (2u * 12u));

  // AFRH for pins 8..15. AF6 = 0b0110
  // PC10 -> AFRH bits [11:8] (index 10-8 = 2)
  GPIOC->AFR[1] &= ~(0xFu << (4u * (10u - 8u)));
  GPIOC->AFR[1] |=  (6u   << (4u * (10u - 8u)));

  // PC12 -> AFRH bits [19:16] (index 12-8 = 4)
  GPIOC->AFR[1] &= ~(0xFu << (4u * (12u - 8u)));
  GPIOC->AFR[1] |=  (6u   << (4u * (12u - 8u)));

  // Optional: high speed
  GPIOC->OSPEEDR |= (3u << (2u * 10u)) | (3u << (2u * 12u));
  // No pull
  GPIOC->PUPDR &= ~((3u << (2u * 10u)) | (3u << (2u * 12u)));

  // --- Configure SPI3 ---
  SPI3->CR1 = 0;   // disable / reset config
  SPI3->CR2 = 0;

  // CR1:
  // MSTR=1 (master)
  // BR=111 (slowest prescaler /256) safe for bring-up
  // CPOL=0, CPHA=0 (SPI mode 0) typical for MCP4921
  // SSM=1, SSI=1 (software NSS)
  SPI3->CR1 |= SPI_CR1_MSTR;
  SPI3->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
  SPI3->CR1 |= (7u << SPI_CR1_BR_Pos); // /256 prescale

  // CR2:
  // DS=1111 => 16-bit data size on many STM32L4 parts
  // (This is the common setting for L4 SPI. If your toolchain
  // complains, tell me and I’ll adjust to the exact bitfield.)
  SPI3->CR2 |= (0xFu << SPI_CR2_DS_Pos);

  // Enable SPI
  SPI3->CR1 |= SPI_CR1_SPE;

  // Optional: set a known starting output, e.g. 0V
  DAC_write(0);
}

void DAC_write(uint16_t code12)
{
  code12 &= 0x0FFF;

  // MCP4921 command bits:
  // bit15: 0 (DAC A)
  // bit14: BUF (0 = unbuffered)
  // bit13: GA  (1 = 1x gain)
  // bit12: SHDN (1 = active)
  uint16_t word = (0u << 15) | (0u << 14) | (1u << 13) | (1u << 12) | code12;

  // CS framing required by lab:
  // CS low before clocks, high after clocks
  cs_low();
  spi3_write16(word);
  cs_high();
}

uint16_t DAC_volt_conv(uint16_t mv, uint16_t vref_mv)
{
  // Clamp
  if (mv >= vref_mv) return 4095u;

  // code = mv * 4095 / vref
  // use 32-bit math to avoid overflow
  uint32_t num = (uint32_t)mv * 4095u;
  uint32_t code = num / (uint32_t)vref_mv;

  if (code > 4095u) code = 4095u;
  return (uint16_t)code;
}

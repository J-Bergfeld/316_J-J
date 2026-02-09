#ifndef DAC_H
#define DAC_H

#include <stdint.h>

/**
 * Initialize SPI3 (PC10=SCK, PC12=MOSI) and CS on PA4.
 * Uses register-level config. Only requires HAL_Init() for HAL_Delay().
 */
void DAC_init(void);

/**
 * Write a 12-bit code to MCP4921.
 * code12: 0..4095
 */
void DAC_write(uint16_t code12);

/**
 * Convert millivolts (mV) to 12-bit DAC code using integer math.
 * vref_mv should match your wiring (usually 3300 mV).
 */
uint16_t DAC_volt_conv(uint16_t mv, uint16_t vref_mv);

#endif

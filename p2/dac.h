#ifndef DAC_H
#define DAC_H

#include <stdint.h>

void DAC_init(void);
void DAC_write(uint16_t code);

#endif

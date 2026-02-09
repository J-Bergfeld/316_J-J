#ifndef TIM2_WAVEOUT_H
#define TIM2_WAVEOUT_H

#include <stdint.h>

void tim2_waveout_init(uint32_t wave_freq_hz);
void tim2_waveout_set_wave_freq(uint32_t wave_freq_hz);

#endif

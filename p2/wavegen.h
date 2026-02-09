#ifndef WAVEGEN_H
#define WAVEGEN_H

#include <stdint.h>

typedef enum {
    WAVE_SINE = 0,
    WAVE_TRIANGLE,
    WAVE_SAW,
    WAVE_SQUARE
} Waveform;

void wavegen_init(void);

void wavegen_set_waveform(Waveform w);
void wavegen_set_frequency_hz(uint32_t hz); // 100..500
void wavegen_set_duty(uint8_t duty);        // 10..90

Waveform wavegen_get_waveform(void);
uint32_t  wavegen_get_frequency_hz(void);
uint8_t   wavegen_get_duty(void);

// called from TIM2 ISR
uint16_t wavegen_next_code(void);

#endif

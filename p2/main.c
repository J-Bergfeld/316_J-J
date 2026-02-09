#include "stm32l476xx.h"
#include <stdint.h>

#include "dac.h"
#include "keypad.h"
#include "wavegen.h"
#include "tim2_waveout.h"

#define STAR  (10)
#define POUND (11)

static void apply_key(int16_t k)
{
    if (k >= 1 && k <= 5) {
        uint32_t f = (uint32_t)k * 100U;
        wavegen_set_frequency_hz(f);
        tim2_waveout_set_wave_freq(f);
        return;
    }

    if (k == 6) { wavegen_set_waveform(WAVE_SINE);     return; }
    if (k == 7) { wavegen_set_waveform(WAVE_TRIANGLE); return; }
    if (k == 8) { wavegen_set_waveform(WAVE_SAW);      return; }
    if (k == 9) { wavegen_set_waveform(WAVE_SQUARE);   return; }

    if (wavegen_get_waveform() == WAVE_SQUARE) {
        if (k == STAR) {
            uint8_t d = wavegen_get_duty();
            d = (d > 10U) ? (uint8_t)(d - 10U) : 10U;
            wavegen_set_duty(d);
        } else if (k == POUND) {
            uint8_t d = wavegen_get_duty();
            d = (d < 90U) ? (uint8_t)(d + 10U) : 90U;
            wavegen_set_duty(d);
        } else if (k == 0) {
            wavegen_set_duty(50U);
        }
    }
}

int main(void)
{
    DAC_init();
    keypad_init();
    wavegen_init();

    wavegen_set_frequency_hz(100U);
    wavegen_set_waveform(WAVE_SQUARE);
    wavegen_set_duty(50U);

    tim2_waveout_init(100U);

    int armed = 1;

    while (1) {
        int16_t k = keypad_getkey();

        if (k == -1) {
            armed = 1;
            continue;
        }

        if (!armed) {
            continue;
        }

        armed = 0;
        apply_key(k);
    }
}

#include "wavegen.h"
#include "wave_tables.h"

static volatile Waveform g_wave = WAVE_SQUARE;
static volatile uint32_t g_freq_hz = 100U;
static volatile uint8_t  g_duty = 50U;

static volatile uint32_t g_idx = 0;

void wavegen_init(void)
{
    g_wave = WAVE_SQUARE;
    g_freq_hz = 100U;
    g_duty = 50U;
    g_idx = 0;
}

void wavegen_set_waveform(Waveform w)
{
    g_wave = w;
    g_idx = 0;
}


void wavegen_set_frequency_hz(uint32_t hz)
{
    g_freq_hz = hz;
}

void wavegen_set_duty(uint8_t duty)
{
    if (duty < 10U) duty = 10U;
    if (duty > 90U) duty = 90U;
    g_duty = duty;
}

Waveform wavegen_get_waveform(void) { return g_wave; }
uint32_t  wavegen_get_frequency_hz(void) { return g_freq_hz; }
uint8_t   wavegen_get_duty(void) { return g_duty; }

uint16_t wavegen_next_code(void)
{
    uint16_t out = 0;

    if (g_wave == WAVE_SQUARE) {
        uint32_t high_count = (WAVE_TABLE_LEN * g_duty) / 100U;
        out = (g_idx < high_count) ? 4095U : 0U;
    } else if (g_wave == WAVE_SINE) {
        out = sine_table[g_idx];
    } else if (g_wave == WAVE_TRIANGLE) {
        out = tri_table[g_idx];
    } else { // WAVE_SAW
        out = saw_table[g_idx];
    }

    g_idx++;
    if (g_idx >= WAVE_TABLE_LEN) g_idx = 0;

    return out;
}

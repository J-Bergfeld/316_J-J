#include "tim2_waveout.h"
#include "stm32l476xx.h"
#include "wave_tables.h"
#include "wavegen.h"
#include "dac.h"

static void tim2_program_rate(uint32_t wave_freq_hz)
{
    uint32_t fs = wave_freq_hz * WAVE_TABLE_LEN;

    uint32_t tim_clk = SystemCoreClock;
    uint32_t psc = 0;
    uint32_t arr = (tim_clk / fs) - 1U;

    while (arr > 0xFFFFU) {
        psc++;
        arr = (tim_clk / (fs * (psc + 1U))) - 1U;
    }

    TIM2->PSC = psc;
    TIM2->ARR = arr;
    TIM2->EGR = TIM_EGR_UG;
}

void tim2_waveout_init(uint32_t wave_freq_hz)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    tim2_program_rate(wave_freq_hz);

    TIM2->DIER |= TIM_DIER_UIE;

    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}

void tim2_waveout_set_wave_freq(uint32_t wave_freq_hz)
{
    tim2_program_rate(wave_freq_hz);
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;

        DAC_write(wavegen_next_code());
    }
}

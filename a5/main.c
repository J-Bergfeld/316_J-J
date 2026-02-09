#include "main.h"
#include "dac.h"

int main(void)
{
  HAL_Init();        // Needed for HAL_Delay() SysTick

  DAC_init();

  // --- One “single transmission” for logic analyzer screenshot ---
  // Example: set ~1.65V (half-scale) so you can comment the capture.
  uint16_t code_half = DAC_volt_conv(1650, 3300);
  DAC_write(code_half);

  HAL_Delay(500);    // gives you time to capture the single transaction if desired

  // --- Required waveform: 25% duty square wave (1V low, 2V high) ---
  uint16_t code_low  = DAC_volt_conv(1000, 3300);  // 1.0V
  uint16_t code_high = DAC_volt_conv(2000, 3300);  // 2.0V

  while (1)
  {
    // 25% high, 75% low
    DAC_write(code_high);
    HAL_Delay(10);   // high time

    DAC_write(code_low);
    HAL_Delay(30);   // low time
  }
}

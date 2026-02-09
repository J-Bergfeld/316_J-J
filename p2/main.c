#include "stm32l4xx_hal.h"

#include <stdint.h>
#include "dac.h"
#include "keypad.h"
#include "wavegen.h"
#include "tim2_waveout.h"


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
        if (k == 10) {
            uint8_t d = wavegen_get_duty();
            d = d-10u;
            wavegen_set_duty(d);
        } else if (k == 11) {
            uint8_t d = wavegen_get_duty();
            d = d+10u;
            wavegen_set_duty(d);
        } else if (k == 0) {
            wavegen_set_duty(50U);
        }
    }
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();
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

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
	Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  //RCC_OscInitStruct.MSIState = RCC_MSI_ON;  //datasheet says NOT to turn on the MSI then change the frequency.
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
	/* from stm32l4xx_hal_rcc.h
	#define RCC_MSIRANGE_0                 MSI = 100 KHz
	#define RCC_MSIRANGE_1                 MSI = 200 KHz
	#define RCC_MSIRANGE_2                 MSI = 400 KHz
	#define RCC_MSIRANGE_3                 MSI = 800 KHz
	#define RCC_MSIRANGE_4                 MSI = 1 MHz
	#define RCC_MSIRANGE_5                 MSI = 2 MHz
	#define RCC_MSIRANGE_6                 MSI = 4 MHz
	#define RCC_MSIRANGE_7                 MSI = 8 MHz
	#define RCC_MSIRANGE_8                 MSI = 16 MHz
	#define RCC_MSIRANGE_9                 MSI = 24 MHz
	#define RCC_MSIRANGE_10                MSI = 32 MHz
	#define RCC_MSIRANGE_11                MSI = 48 MHz   dont use this one*/
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;  //datasheet says NOT to turn on the MSI then change the frequency.
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
	Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
	Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}


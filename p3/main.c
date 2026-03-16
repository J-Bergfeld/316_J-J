/*
 * main.c
 *
 * Top-level application control.
 * Handles keypad input and maps user actions to waveform
 * generation and timer-driven DAC output.
 */

#include "stm32l4xx_hal.h"
#include <stdint.h>

#include "dac.h"
#include "keypad.h"
#include "wavegen.h"
#include "tim2_waveout.h"

/* ---------- Configuration Constants ---------- */

#define BASE_FREQUENCY_HZ          (100U)

#define DEFAULT_FREQUENCY_HZ       (100U)
#define DEFAULT_DUTY_PERCENT       (50U)

#define KEY_NONE                   (-1)

#define KEY_FREQ_MIN               (1)
#define KEY_FREQ_MAX               (5)

#define KEY_WAVE_SINE              (6)
#define KEY_WAVE_TRIANGLE          (7)
#define KEY_WAVE_SAW               (8)
#define KEY_WAVE_SQUARE            (9)

#define KEY_DUTY_DOWN              (10)
#define KEY_DUTY_UP                (11)
#define KEY_DUTY_RESET             (0)

#define DUTY_STEP_PERCENT          (10U)

/* ---------- Local Worker Functions ---------- */

/*
 * Applies a validated keypad key to waveform configuration.
 * Assumes debouncing / edge detection has already occurred.
 */
static void apply_key(int16_t key)
{
    uint32_t frequencyHz;
    uint8_t  dutyPercent;

    /* Frequency selection: keys 1–5 map to 100–500 Hz */
    if ((key >= KEY_FREQ_MIN) && (key <= KEY_FREQ_MAX)) {
        frequencyHz = (uint32_t)key * BASE_FREQUENCY_HZ;
        wavegen_set_frequency_hz(frequencyHz);
        tim2_waveout_set_wave_freq(frequencyHz);
        return;
    }

    /* Waveform selection */
    if (key == KEY_WAVE_SINE) {
        wavegen_set_waveform(WAVE_SINE);
        return;
    }

    if (key == KEY_WAVE_TRIANGLE) {
        wavegen_set_waveform(WAVE_TRIANGLE);
        return;
    }

    if (key == KEY_WAVE_SAW) {
        wavegen_set_waveform(WAVE_SAW);
        return;
    }

    if (key == KEY_WAVE_SQUARE) {
        wavegen_set_waveform(WAVE_SQUARE);
        return;
    }

    /* Duty cycle control applies only to square wave */
    if (wavegen_get_waveform() != WAVE_SQUARE) {
        return;
    }

    dutyPercent = wavegen_get_duty();

    if (key == KEY_DUTY_DOWN) {
        wavegen_set_duty(dutyPercent - DUTY_STEP_PERCENT);
    } else if (key == KEY_DUTY_UP) {
        wavegen_set_duty(dutyPercent + DUTY_STEP_PERCENT);
    } else if (key == KEY_DUTY_RESET) {
        wavegen_set_duty(DEFAULT_DUTY_PERCENT);
    }
}

/* ---------- Main ---------- */

int main(void)
{
    int16_t keyValue;
    uint8_t keyArmed;

    HAL_Init();
    SystemClock_Config();

    DAC_Init();
    keypad_init();
    wavegen_init();

    /* Default waveform configuration */
    wavegen_set_frequency_hz(DEFAULT_FREQUENCY_HZ);
    wavegen_set_waveform(WAVE_SQUARE);
    wavegen_set_duty(DEFAULT_DUTY_PERCENT);

    tim2_waveout_init(DEFAULT_FREQUENCY_HZ);

    /* Prevent repeated key actions while key is held */
    keyArmed = 1U;

    while (1) {
        keyValue = keypad_getkey();

        /* No key pressed: re-arm */
        if (keyValue == KEY_NONE) {
            keyArmed = 1U;
            continue;
        }

        /* Ignore held key until released */
        if (keyArmed == 0U) {
            continue;
        }

        keyArmed = 0U;
        apply_key(keyValue);
    }
}

/* ------------------------------------------------------------------
 * SystemClock_Config
 * Provided
 * ------------------------------------------------------------------ */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    if (HAL_PWREx_ControlVoltageScaling(
            PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK  |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/*
 * Error handler: halts execution on unrecoverable fault.
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1) {
    }
}

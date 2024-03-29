
#include <cstdint>
#include "stm32l4xx_hal.h"
// #include "stm32l4xx_ll_gpio.h"
// #include "stm32l4xx_nucleo_32.h"

#define LED3_PIN GPIO_PIN_3
#define LED3_GPIO_PORT GPIOB

#define LED3_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

namespace
{
    void system_clock_init()
    {

        /*
         *         The system Clock is configured as follow :
         *            System Clock source            = PLL (MSI)
         *            SYSCLK(Hz)                     = 80000000
         *            HCLK(Hz)                       = 80000000
         *            AHB Prescaler                  = 1
         *            APB1 Prescaler                 = 1
         *            APB2 Prescaler                 = 1
         *            MSI Frequency(Hz)              = 4000000
         *            PLL_M                          = 1
         *            PLL_N                          = 40
         *            PLL_R                          = 2
         *            PLL_P                          = 7
         *            PLL_Q                          = 4
         *            Flash Latency(WS)              = 4
         */
        RCC_ClkInitTypeDef RCC_ClkInitStruct;
        RCC_OscInitTypeDef RCC_OscInitStruct;

        /* MSI is enabled after System reset, activate PLL with MSI as source */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
        RCC_OscInitStruct.MSIState = RCC_MSI_ON;
        RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
        RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
        RCC_OscInitStruct.PLL.PLLM = 1;
        RCC_OscInitStruct.PLL.PLLN = 40;
        RCC_OscInitStruct.PLL.PLLR = 2;
        RCC_OscInitStruct.PLL.PLLP = 7;
        RCC_OscInitStruct.PLL.PLLQ = 4;
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        {
            /* Initialization Error */
            while (1)
                ;
        }

        /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
           clocks dividers */
        RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
        {
            /* Initialization Error */
            while (1)
                ;
        }
    }

    void gpio_init()
    {

        LED3_GPIO_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct;

        /* -2- Configure IO in output push-pull mode to drive external LEDs */
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

        GPIO_InitStruct.Pin = LED3_PIN;
        HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);
    }
} // namespace

int main()
{

    /**
     * STM32L4xx HAL library initialization:
     * - Configure the Flash prefetch, Flash preread and Buffer caches
     * - Systick timer is configured by default as source of time base, but user
     *     can eventually implement his proper time base source (a general purpose
     *     timer for example or other time source), keeping in mind that Time base
     *     duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
     *     handled in milliseconds basis.
     * - Low Level Initialization
     */
    HAL_Init();

    /* Configure the System clock to have a frequency of 80 MHz */
    system_clock_init();
    gpio_init();

    /* -1- Enable GPIO Clock (to be able to program the configuration registers) */

    /* -3- Toggle IO in an infinite loop */
    while (1)
    {
        HAL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_PIN);
        /* Insert delay 100 ms */
        HAL_Delay(1000);
    }
}

extern "C"
{

#ifdef USE_FULL_ASSERT

    /**
     * @brief  Reports the name of the source file and the source line number
     *         where the assert_param error has occurred.
     * @param  file: pointer to the source file name
     * @param  line: assert_param error line source number
     * @retval None
     */
    void assert_failed(uint8_t *file, uint32_t line)
    {
        /* User can add his own implementation to report the file name and line number,
           ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

        /* Infinite loop */
        for (;;);
    }
#endif

} // extern "C"
// #define USARTx                           USART1
// #define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
// #define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
// #define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

// #define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
// #define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

// /* Definition for USARTx Pins */
// #define USARTx_TX_PIN                    GPIO_PIN_9
// #define USARTx_TX_GPIO_PORT              GPIOA
// #define USARTx_TX_AF                     GPIO_AF7_USART1
// #define USARTx_RX_PIN                    GPIO_PIN_10
// #define USARTx_RX_GPIO_PORT              GPIOA
// #define USARTx_RX_AF                     GPIO_AF7_USART1


#include <cstdint>
#include <string_view>
#include <array>
#include <charconv>


#include "stm32l4xx_hal.h"

// #include <fmt/format.h>




namespace {
struct GpioOutputPin {
    GPIO_TypeDef * port;
    uint16_t pin;

    void toggle() {
        HAL_GPIO_TogglePin(this->port, this->pin);
    }
    void set() {
        HAL_GPIO_WritePin(this->port, this->pin, GPIO_PinState::GPIO_PIN_SET);
    }
    void reset() {
        HAL_GPIO_WritePin(this->port, this->pin, GPIO_PinState::GPIO_PIN_RESET);
    }
};

struct USART {
    USART_HandleTypeDef port;
    bool init() {
        return HAL_USART_Init(&this->port) != HAL_OK;
    }

    bool deinit() {
        return HAL_USART_DeInit(&this->port) != HAL_OK;
    }

    bool transmit(const void * data, uint16_t size, uint32_t timeout = 0xffffffff) {
        return HAL_USART_Transmit(&this->port, reinterpret_cast<const uint8_t*>(data), size, timeout) != HAL_OK;
    }
};
class Application {
private:
    USART usart {
        .port{
            .Instance{USART2},
            .Init{
                .BaudRate{115200},
                .WordLength{UART_WORDLENGTH_8B},
                .StopBits{UART_STOPBITS_1},
                .Parity{UART_PARITY_NONE},
                .Mode{UART_MODE_TX_RX},
            }
        }
    };

    GpioOutputPin led {
        .port{GPIOB},
        .pin{GPIO_PIN_3},
    };

    uint32_t tickCount{0};


    void initSystemClock_() {

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
            while (1);
        }
    }

    void initGPIO_() {

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitTypeDef gpioConf{
            .Pin{led.pin},
            .Mode{GPIO_MODE_OUTPUT_PP},
            .Pull{GPIO_NOPULL},
            .Speed{GPIO_SPEED_FREQ_VERY_HIGH}
        };

        HAL_GPIO_Init(led.port, &gpioConf);

        /**
         * USART2
         * PA2 - USART2_TX (AF7)
         * PA15 - USART2_RX (AF3)
         */
        gpioConf.Pin = GPIO_PIN_2;
        gpioConf.Mode = GPIO_MODE_AF_PP;
        gpioConf.Pull = GPIO_NOPULL;
        gpioConf.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpioConf.Alternate = 7;
        HAL_GPIO_Init(GPIOA, &gpioConf);

        gpioConf.Pin = GPIO_PIN_15;
        gpioConf.Alternate = 3;
        HAL_GPIO_Init(GPIOA, &gpioConf);


    }

    void errorHandler_() {
        led.toggle();
        /* Insert delay 100 ms */
        HAL_Delay(100);
    }

    void initUSART_() {
        __HAL_RCC_USART2_CLK_ENABLE();

        if(usart.deinit()) {
            errorHandler_();
        }
        if(usart.init()) {
            errorHandler_();
        }
    }


public:
    Application() {
        HAL_Init();
        initSystemClock_();
        initGPIO_();
        initUSART_();
    }

    void tick() {
        usart.transmit("[", 1);

        std::array<char, 100> messageBuffer;
        auto res = std::to_chars(messageBuffer.data(), messageBuffer.data() + messageBuffer.size(), tickCount);
        std::string_view numericMessage{messageBuffer.data(), res.ptr};
        usart.transmit(numericMessage.data(), numericMessage.size());

        const std::string_view message{"] Hello from stm32!\n"};
        usart.transmit(message.data(), message.size());

        led.toggle();
        ++tickCount;
        HAL_Delay(1000);
    }

}; // class Application

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


    /* Configure the System clock to have a frequency of 80 MHz */
    Application app;






    /* -1- Enable GPIO Clock (to be able to program the configuration registers) */

    /* -3- Toggle IO in an infinite loop */
    while (1)
    {
        app.tick();

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
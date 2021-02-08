/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_config.h"
#include "ticker.h"
#include "frame.h"
#include "frame_com.h"
#include "uart_com_app.h"
#include "app_uart_fifo.h"
#include "io_input.h"
#include "input_service.h"
#include "at_cmd.h"
#include "at_cmd_process.h"
#include "serial_console_dbg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define EVSE_FC_RX_BUFF_SIZE    FRAME_SIZE_MAX
#define EVSE_FC_TX_BUFF_SIZE    FRAME_SIZE_MAX
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
static frame_com_cxt_t frame_com_uart_cxt;
static at_cmd_cxt_t at_cmd_uart_cxt;
static at_cmd_callback_handle_t at_cmd_callback_handle;
static if_callback_handle_t evse_callback_handle;

static io_input_cxt_t button_cxt = IO_INPUT_DEFAULT;
static service_io_input_handle_t button_service = SERVICE_IO_INPUT_DEFAULT;
static ticker_function_handle_t tick_button_sample;

static ticker_function_handle_t tick_led_status;

static uint8_t jig_process_enable = 0;

static char const *button_serviceid[] = {
/* 0 */"IO_INPUT_FALLING",
/* 1 */"IO_INPUT_RISING",
/* 2 */"BUTTON_SINGER_EVT",
/* 3 */"BUTTON_DOUBLE_EVT",
/* 4 */"BUTTON_HOLD_ON_EVT",
/* 5 */"BUTTON_IDLE_EVT" };

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
extern void uart_irq_init(void);
extern void uart_error_irq_process(UART_HandleTypeDef *huart,
		app_uart_irq_t irq_event);
extern void uart_tx_irq_process(UART_HandleTypeDef *huart,
		app_uart_irq_t irq_event);
extern void uart_rx_irq_process(UART_HandleTypeDef *huart,
		app_uart_irq_t irq_event);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	uart_error_irq_process(huart, APP_UART_IRQ_ERROR);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	uart_tx_irq_process(huart, APP_UART_IRQ_TX);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	uart_rx_irq_process(huart, APP_UART_IRQ_RX);
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	uart_rx_irq_process(huart, APP_UART_IRQ_IDLE);
}

void USART_IDLE_line_IRQHandler(UART_HandleTypeDef *huart) {
	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) {
		/* Clear IDLE line flag */
		__HAL_UART_CLEAR_IDLEFLAG(huart);

		uart_rx_irq_process(huart, APP_UART_IRQ_IDLE);
	}
}

static uint8_t jig_query(void)
{
  return (1 == jig_process_enable);
}

static void jig_setup(uint8_t jig)
{
  jig_process_enable = jig;
  if (jig)
  {
    DBG_PRINTF("\r\nJIG Enable");
  }
  else
  {
    DBG_PRINTF("\r\nJIG Disable");
  }

  DBG_PRINTF("\r\n");
}

static void led_status_blink_cb(uint32_t remain) {
	HAL_GPIO_TogglePin(GPIOB, LED_STATUS_Pin);
}

/*  */
static void button_sample_rate_cb(uint32_t remain) {
	uint8_t ip_status;
	(void) remain;

	ip_status = HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
	io_input_process(&button_cxt, ip_status);
}

static void button_service_cb(service_io_input_handle_t *service, uint8_t evt) {
	DBG_PRINTF("\r\nButton service: %s", button_serviceid[evt]);
	DBG_PRINTF("\r\n");
	if (service == &button_service) {
		switch (evt) {
		case IO_INPUT_FALLING:
			break;

		case IO_INPUT_RISING:
			break;

		case BUTTON_SINGER_EVT:
			break;

		case BUTTON_DOUBLE_EVT:
			break;

		case BUTTON_HOLD_ON_EVT:
			break;

		case BUTTON_IDLE_EVT:
			break;

		default:
			break;
		}
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */

	uart_irq_init();

	/* Register serial interface with rfid */
	frame_com_uart_cxt.instance = FRAME_UART_INTERFACE;
	frame_com_uart_cxt.input_cb = read_uart_instance3;
	frame_com_uart_cxt.output_cb = write_uart_instance3;
	/* Init frame and process receive command callback function */
	FRAME_COM_INIT(&frame_com_uart_cxt, if_receive_cmd_callback,
			EVSE_FC_RX_BUFF_SIZE, EVSE_FC_TX_BUFF_SIZE);

	evse_callback_handle.jig_query_cb = jig_query;  /* Register jig query */
  evse_callback_handle.jig_setup_cb = jig_setup;  /* Register jig setup */
  if_callback_register(&evse_callback_handle);

	/* Init at command process */
  /*--------------------------------------------------------------------------*/
  at_cmd_uart_cxt.input_cb = read_uart_instance3;
  at_cmd_uart_cxt.output_cb = write_uart_instance3;
  at_cmd_uart_cxt.cmd_table = at_fun_handle;
  at_cmd_uart_cxt.cmd_num = AT_CMD_HANDLE_NUM;
  /* Init uart at command with 32 bytes buffer */
  AT_DEVICE_INIT(&at_cmd_uart_cxt, 32);

  at_cmd_callback_handle.jig_query_cb = jig_query;  /* Register jig query */
  at_cmd_callback_handle.jig_setup_cb = jig_setup;  /* Register jig setup */
  at_cmd_callback_register(&at_cmd_callback_handle);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	button_service.evt_cb = button_service_cb;
	button_service.edge_release = IO_FALLING;
	button_service.level_active = IO_HIGH;
	io_input_init(&button_cxt, 5, 5); /* 5 is high and low sample rate count*/
	service_io_input_init(&button_cxt, &button_service);
	/* Init tick sample rate 10ms */
	ticker_function_init(&tick_button_sample, button_sample_rate_cb, 10,
			TICKER_FOREVER);

	/* Init tick led status blink */
	ticker_function_init(&tick_led_status, led_status_blink_cb, 300,
			TICKER_FOREVER);

	DBG_PRINTF("\r\nRFID Reader charging SampleA module V%u.%u.%u\r\n",
	      FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_BUILD);
	while (1) {
    /* timer ticker handler */
    ticker_loop();

    if (!jig_process_enable)
    {
      /* EVSE interface handle */
      frame_com_process(&frame_com_uart_cxt);
    }
    else
    {
      /* JIG AT command interface */
      at_cmd_capture(&at_cmd_uart_cxt);
    }

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	/** Supply configuration update enable
	 */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 120;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1
			| RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
	PeriphClkInitStruct.Usart234578ClockSelection =
			RCC_USART234578CLKSOURCE_D2PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void) {

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart3) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	/* DMA1_Stream3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, LED_CAPTURE_Pin | GPIO_PIN_5, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, LED_ERROR_Pin | LED_STATUS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin : USER_BUTTON_Pin */
	GPIO_InitStruct.Pin = USER_BUTTON_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : RMII_MDC_Pin */
	GPIO_InitStruct.Pin = RMII_MDC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(RMII_MDC_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LED_CAPTURE_Pin */
	GPIO_InitStruct.Pin = LED_CAPTURE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(LED_CAPTURE_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : D0_Pin D1_Pin D2_Pin D3_Pin
	 D4_Pin D5_Pin D6_Pin D7_Pin
	 A0_Pin A1_Pin A2_Pin A3_Pin */
	GPIO_InitStruct.Pin = D0_Pin | D1_Pin | D2_Pin | D3_Pin | D4_Pin | D5_Pin
			| D6_Pin | D7_Pin | A0_Pin | A1_Pin | A2_Pin | A3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : WR_Pin */
	GPIO_InitStruct.Pin = WR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(WR_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PC5 */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : DD0_Pin DD1_Pin DD2_Pin */
	GPIO_InitStruct.Pin = DD0_Pin | DD1_Pin | DD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : RMII_TXD1_Pin */
	GPIO_InitStruct.Pin = RMII_TXD1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(RMII_TXD1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED_ERROR_Pin LED_STATUS_Pin */
	GPIO_InitStruct.Pin = LED_ERROR_Pin | LED_STATUS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : USB_PowerSwitchOn_Pin */
	GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : USB_OverCurrent_Pin */
	GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : USB_DP_Pin */
	GPIO_InitStruct.Pin = USB_DP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
	HAL_GPIO_Init(USB_DP_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : RMII_TX_EN_Pin RMII_TXD0_Pin */
	GPIO_InitStruct.Pin = RMII_TX_EN_Pin | RMII_TXD0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/**/
	HAL_I2CEx_EnableFastModePlus(SYSCFG_PMCR_I2C_PB7_FMP);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

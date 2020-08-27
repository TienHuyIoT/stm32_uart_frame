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
#include "serial0.h"
#include "ticker.h"
#include "frame.h"
#include "frame_com.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
    UART_CMD_CAN                        = 1,
    UART_CMD_TIME                       = 2,
    UART_CMD_PASS_KEY                   = 3,
    UART_CMD_KEY_ADDR_0                 = 4,
    UART_CMD_KEY_ADDR_1                 = 5,
    UART_CMD_FIND                       = 6,
    UART_CMD_LOCK                       = 7,
    UART_CMD_UNLOCK                     = 8,
    UART_CMD_RSSI                       = 9,
    UART_CMD_BONDING                    = 10,
    UART_CMD_POWER_ON                   = 11,
    UART_CMD_POWER_OFF                  = 12,
    UART_CMD_ACK                        = 13,
    UART_CMD_NACK                       = 14,
    UART_CMD_LONG_ITV                   = 15,
    UART_CMD_SHORT_ITV                  = 16,
    UART_CMD_KEY_LONG_ITV               = 17,
    UART_CMD_KEY_SHORT_ITV              = 18,
    UART_CMD_BLE_CONNECTED              = 19,
    UART_CMD_BLE_DISCONNECTED           = 20,
    UART_CMD_BLE_MESSAGE                = 21,
    UART_CMD_BLE_INCOMING_CALL          = 22,
    UART_CMD_HEART_BEAT                 = 23,
    UART_CMD_BLE_NAME                   = 24,
    UART_CMD_READ_KEY_INFO_0            = 25,
    UART_CMD_READ_KEY_INFO_1            = 26,
    UART_CMD_LOCKHOLD                   = 27,
    REMOTE_WAKEUP_CHARGER_SIG           = 28,
    HMI_LF_ENABLE_SIG                   = 29,
    REMOTE_LF_AUTHENTICATED             = 30,
    UART_CMD_FW_VERSION                 = 31,
    UART_CMD_GET_KEY_SCAN_LF            = 32,
    UART_CMD_FW_ENTER_UPDATE            = 33,
    UART_CMD_APP_AUTH                   = 34,
    UART_CMD_OPEN_SADDLE                = 35,
    UART_CMD_SMK_BATTERY                = 36,
	UART_CMD_NUM						= 37
} serial_command_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define COMMAND_RX_BUFF_SIZE FRAME_SIZE_MAX
#define COMMAND_TX_BUFF_SIZE FRAME_SIZE_MAX
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */
/**@brief String literals for the serial command. */
static char const * lit_serialid[] = {
    "",
    "UART_CMD_CAN",
    "UART_CMD_TIME",
    "UART_CMD_PASS_KEY",
    "UART_CMD_KEY_ADDR_0",
    "UART_CMD_KEY_ADDR_1",
    "UART_CMD_FIND",
    "UART_CMD_LOCK",
    "UART_CMD_UNLOCK",
    "UART_CMD_RSSI",
    "UART_CMD_BONDING",
    "UART_CMD_POWER_ON",
    "UART_CMD_POWER_OFF",
    "UART_CMD_ACK",
    "UART_CMD_NACK",
    "UART_CMD_LONG_ITV",
    "UART_CMD_SHORT_ITV",
    "UART_CMD_KEY_LONG_ITV",
    "UART_CMD_KEY_SHORT_ITV",
    "UART_CMD_BLE_CONNECTED",
    "UART_CMD_BLE_DISCONNECTED",
    "UART_CMD_BLE_MESSAGE",
    "UART_CMD_BLE_INCOMING_CALL",
    "UART_CMD_HEART_BEAT",
    "UART_CMD_BLE_NAME",
    "UART_CMD_READ_KEY_INFO_0",
    "UART_CMD_READ_KEY_INFO_1",
    "UART_CMD_LOCKHOLD",
    "REMOTE_WAKEUP_CHARGER_SIG",
    "HMI_LF_ENABLE_SIG",
    "REMOTE_LF_AUTHENTICATED",
    "UART_CMD_FW_VERSION",
    "UART_CMD_GET_KEY_SCAN_LF",
    "UART_CMD_FW_ENTER_UPDATE",
    "UART_CMD_APP_AUTH",
    "UART_CMD_OPEN_SADDLE",
    "UART_CMD_SMK_BATTERY"
};
uint8_t command_rx_buff[COMMAND_RX_BUFF_SIZE];
uint8_t command_tx_buff[COMMAND_TX_BUFF_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
static void command_receive_event_handle(uint8_t result, uint8_t cmd, uint8_t* data, uint16_t length);
static void command_send_callback(uint8_t* buff, uint16_t length);
static void command_parse_process(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  frame_com_cxt_t frame_com_cxt;
  uint8_t data_buf[4] = {'1' , '2', '3', '4'};
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

  /* Init uart module */
  uart_instance0_Init();

  /* Init object handle communicate frame*/
  frame_com_cxt.rx_buff   = command_rx_buff;
  frame_com_cxt.rx_length = COMMAND_RX_BUFF_SIZE;
  frame_com_cxt.tx_buff   = command_tx_buff;
  frame_com_cxt.tx_length = COMMAND_TX_BUFF_SIZE;
  frame_com_cxt.event_cb  = command_receive_event_handle;
  frame_com_cxt.send_cb   = command_send_callback;
  frame_com_begin(&frame_com_cxt);

  /* API send command (cmd, *data, length)
   * frame command $7E$06$011234$03$7F using "hercules terminal" for test parse
  */
  frame_com_send((uint8_t)UART_CMD_CAN, data_buf, sizeof(data_buf));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  _PRINTF("\r\nStart serial communication with frame format V1.0.0\n\r");
  while (1)
  {
	  /* Get command event process */
	  command_parse_process();

	  /* timer ticker process handler */
	  ticker_loop();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable 
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the CPU, AHB and APB busses clocks 
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

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
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_CAPTURE_Pin|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_ERROR_Pin|LED_STATUS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

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
  GPIO_InitStruct.Pin = D0_Pin|D1_Pin|D2_Pin|D3_Pin 
                          |D4_Pin|D5_Pin|D6_Pin|D7_Pin 
                          |A0_Pin|A1_Pin|A2_Pin|A3_Pin;
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
  GPIO_InitStruct.Pin = DD0_Pin|DD1_Pin|DD2_Pin;
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
  GPIO_InitStruct.Pin = LED_ERROR_Pin|LED_STATUS_Pin;
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
  GPIO_InitStruct.Pin = RMII_TX_EN_Pin|RMII_TXD0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /**/
  HAL_I2CEx_EnableFastModePlus(SYSCFG_PMCR_I2C_PB7_FMP);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

static void command_parse_process(void)
{
    uint8_t data;
	uint8_t get_cnt = 50; /* counter limit once getchar from uart */
	while (uart_instance0_available())
	{
		(void)getchar_instance0((char *)(&data));
		frame_com_getchar(data);

		--get_cnt;
		if(0 == get_cnt) {
			break;
		}
	}
}

static void command_send_callback(uint8_t* buff, uint16_t length)
{
	_PRINTF("\r\nSend frame command with length = %u\r\n", length);
	for(uint16_t i = 0; i < length; ++i)
	{
		putchar_instance0(buff[i]);
	}
	_PRINTF("\r\n");
}

static void command_receive_event_handle(uint8_t result, uint8_t cmd, uint8_t* data, uint16_t length)
{
	if((uint8_t)FRAME_OK == result)
	{
		if(UART_CMD_NUM <= cmd)
		{
			_PRINTF("\r\nCMD error\r\n");
			return;
		}

		_PRINTF("\r\nGet new frame");
		_PRINTF("\r\n- CMD: %s", lit_serialid[cmd]);
		_PRINTF("\r\n- Data length: %u", length);
		_PRINTF("\r\n- Data: ");
		for(uint16_t i = 0; i < length; ++i)
		{
			_PRINTF("%02X ", data[i]);
		}
		_PRINTF("\r\n");

		switch (cmd)
		{
		case UART_CMD_CAN:
			break;

		default:
			break;
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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

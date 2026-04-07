/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "lmic.h"
#include "../../Middleware/ThirdParty/LMIC/hal/hal.h"

/*
 * Red Led : A3 = PA4
 * Yellow Led: D11 = PB5
 * Green LED: D12 = PB4
 *
 * Dip Switch:
 * bit 1: wrong: D7, correction: D13 = PB3
 * bit 2: wrong: D8, correction: A1 = PA1
 * bit 3: D9 = PA8
 * bit 4: D10 = PA11
 *
 * BUTTON:
 * D4 = PB7
 */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* --- GPS & UART Variables --- */
uint8_t rx_byte;
uint8_t gps_rx_buffer[100];
volatile uint16_t rx_index = 0;
volatile uint8_t sentence_ready = 0;

// --- Fleet Tracking Variables ---
volatile uint8_t emergency_mode = 0;    // 0 = Normal, 1 = Emergency
volatile uint32_t last_button_press = 0; // For debouncing
unsigned tx_interval = 60;              // Defaults to 60 seconds

/* --- LMIC Timing & Hardware --- */
static osjob_t sendjob;

const lmic_pinmap lmic_pins = {
    .nss = 0,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 0,
    .dio = {0, 0, LMIC_UNUSED_PIN},
};

/* --- LoRaWAN Keys & Fleet Identity --- */
void os_getArtEui (u1_t* buf) { memcpy(buf, "\x00\x00\x00\x00\x00\x00\x00\x00", 8); }
void os_getDevKey (u1_t* buf) { memcpy(buf, "\xd1\xf6\x4f\xda\xef\x7d\xf2\xa8\xa4\x23\xc6\x35\x9a\x00\xcc\x2e", 16); }

typedef struct {
    uint8_t devEui[8];
} TrackerIdentity;

const TrackerIdentity Fleet[16] = {
    {{0xe7, 0x8e, 0x5f, 0xa8, 0x26, 0xcc, 0x93, 0x10}}, // ID 0
    {{0x0d, 0x0a, 0x23, 0x30, 0x78, 0x94, 0x87, 0x6f}}, // ID 1
    {{0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}}  // ID 2
    // Add remaining DevEUIs here
};

uint8_t active_tracker_id = 0;

uint8_t read_dip_switches(void) {
    uint8_t id = 0;
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET) id |= (1 << 0);
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_SET) id |= (1 << 1);
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET) id |= (1 << 2);
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_SET) id |= (1 << 3);
    return id;
}

void os_getDevEui (u1_t* buf) {
    memcpy(buf, Fleet[active_tracker_id].devEui, 8);
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* --- GPS Code --- */
uint8_t rxBuffer[128] = {0};
uint8_t rxIndex = 0;
uint8_t rxData;
float nmeaLong;
float nmeaLat;
float utcTime;
char northsouth;
char eastwest;
char posStatus;
float decimalLong;
float decimalLat;

float nmeaToDecimal(float coordinate) {
    int degree = (int)(coordinate/100);
    float minutes = coordinate - degree * 100;
    float decimalDegree = minutes / 60;
    float decimal = degree + decimalDegree;
    return decimal;
}

void gpsParse(char *strParse){
  if(!strncmp(strParse, "$GNGGA", 6)){
    sscanf(strParse, "$GNGGA,%f,%f,%c,%f,%c",
      &utcTime, &nmeaLat, &northsouth, &nmeaLong, &eastwest);
    decimalLat = nmeaToDecimal(nmeaLat);
    decimalLong = nmeaToDecimal(nmeaLong);
	sentence_ready = 1;
  }
  else if (!strncmp(strParse, "$GNGLL", 6)){
    sscanf(strParse, "$GNGLL,%f,%c,%f,%c,%f",
      &nmeaLat, &northsouth, &nmeaLong, &eastwest, &utcTime);
    decimalLat = nmeaToDecimal(nmeaLat);
    decimalLong = nmeaToDecimal(nmeaLong);
    sentence_ready = 1;
  }
  else if (!strncmp(strParse, "$GNRMC", 6)){
    sscanf(strParse, "$GNRMC,%f,%c,%f,%c,%f,%c",
      &utcTime, &posStatus, &nmeaLat, &northsouth, &nmeaLong, &eastwest);
    decimalLat = nmeaToDecimal(nmeaLat);
    decimalLong = nmeaToDecimal(nmeaLong);
	sentence_ready = 1;
  }

  if (sentence_ready == 1) {
	  if (decimalLat != 0 && decimalLong != 0) {
	      HAL_GPIO_WritePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin, GPIO_PIN_SET);
	  } else {
	      HAL_GPIO_WritePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin, GPIO_PIN_RESET);
	  }
  }
}

int gpsValidate(char *nmea){
    char check[3];
    char calculatedString[3];
    int index;
    int calculatedCheck;

    index=0;
    calculatedCheck=0;

    // Ensure that the string starts with a "$"
    if(nmea[index] == '$')
        index++;
    else
        return 0;

    //No NULL reached, 75 char largest possible NMEA message, no '*' reached
    while((nmea[index] != 0) && (nmea[index] != '*') && (index < 75)){
        calculatedCheck ^= nmea[index];// calculate the checksum
        index++;
    }

    if(index >= 75){
        return 0;// the string is too long so return an error
    }

    if (nmea[index] == '*'){
        check[0] = nmea[index+1];    //put hex chars in check string
        check[1] = nmea[index+2];
        check[2] = 0;
    }
    else
        return 0;// no checksum separator found therefore invalid data

    sprintf(calculatedString,"%02X",calculatedCheck);
    return((calculatedString[0] == check[0])
        && (calculatedString[1] == check[1])) ? 1 : 0 ;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance==USART1)
  {
    // if the character received is other than 'enter' ascii13, save the data in buffer
    if(rxData!='\n' && rxIndex < sizeof(rxBuffer))
    {
      rxBuffer[rxIndex++]=rxData;
    }
    else
    {
      if(gpsValidate((char*) rxBuffer)){
    	  gpsParse((char*) rxBuffer);
      }
      rxIndex=0;
      memset(rxBuffer,0,sizeof(rxBuffer));
    }
    HAL_UART_Receive_IT(&huart1,&rxData,1); // Enabling interrupt receive again
  }
}

/* --- LoRaWAN Transmission & Event Logic --- */
void do_send(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        printf("OP_TXRXPEND, not sending\n");
    } else {
        int32_t lat_int = (int32_t)(decimalLat * 100000);
        int32_t lon_int = (int32_t)(decimalLong * 100000);
        uint8_t payload[9];
        // Byte 0: Emergency Status (0 or 1)
        payload[0] = emergency_mode;

        // Bytes 1-4: Latitude
        payload[1] = (lat_int >> 24) & 0xFF;
        payload[2] = (lat_int >> 16) & 0xFF;
        payload[3] = (lat_int >> 8) & 0xFF;
        payload[4] = lat_int & 0xFF;

        // Bytes 5-8: Longitude
        payload[5] = (lon_int >> 24) & 0xFF;
        payload[6] = (lon_int >> 16) & 0xFF;
        payload[7] = (lon_int >> 8) & 0xFF;
        payload[8] = lon_int & 0xFF;

        // Send 9 bytes!
        LMIC_setTxData2(1, payload, sizeof(payload), 0);
    }
}

void onEvent (ev_t ev) {
    switch(ev) {
        case EV_JOINING:
            printf("Joining...\n");
            HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
            break;
        case EV_JOINED:
            printf("Join Success.\n");
            HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
            LMIC_setLinkCheckMode(0);
            do_send(&sendjob);
            break;

        case EV_JOIN_FAILED:
            printf("Join Failed.\n");
            HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
            break;

        case EV_TXCOMPLETE:
            printf("Uplink complete.\n");
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(tx_interval + (rand() % 10)), do_send);
            break;

        default:
            break;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  /* --- System Initialization --- */
    active_tracker_id = read_dip_switches();
    printf("Booting Tracker ID: %d\n", active_tracker_id);

    HAL_UART_Receive_IT(&huart1, &rxData, 1);

    os_init();
    LMIC_reset();
    LMIC_setClockError(MAX_CLOCK_ERROR * 5 / 100);

    // Timing Tester code
    /*ostime_t next_10s_tick = os_getTime() + sec2osticks(10);
    int tick_count = 0;*/

    do_send(&sendjob);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* --- Main Execution Loop --- */
        os_runloop_once();

        /*
         Timing Tester code
        if ((s4_t)(os_getTime() - next_10s_tick) >= 0) {

		  tick_count++;
		  printf("TICK %d: 10 LMIC seconds have passed.\n", tick_count);

		  // Toggle the Green LED to give you a visual stopwatch cue
		  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

		  // Schedule the next tick exactly 10 seconds from the LAST target
		  next_10s_tick = os_getTime() + sec2osticks(10);
	  	}*/
        if (sentence_ready == 1) {
            sentence_ready = 0;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 79;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RED_LED_Pin|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GREEN_LED_Pin|YELLOW_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DIP_SW_2_Pin DIP_SW_3_Pin DIP_SW_4_Pin */
  GPIO_InitStruct.Pin = DIP_SW_2_Pin|DIP_SW_3_Pin|DIP_SW_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RED_LED_Pin PA12 */
  GPIO_InitStruct.Pin = RED_LED_Pin|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 GREEN_LED_Pin YELLOW_LED_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GREEN_LED_Pin|YELLOW_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 DIP_SW_1_Pin PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|DIP_SW_1_Pin|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_INT_Pin */
  GPIO_InitStruct.Pin = BUTTON_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_INT_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == BUTTON_INT_Pin) {

        uint32_t current_time = HAL_GetTick();

        if (current_time - last_button_press > 15000) {
            last_button_press = current_time;

            emergency_mode = !emergency_mode;

            if (emergency_mode) {
                tx_interval = 15;
                HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
            } else {
                tx_interval = 60; // Return to normal interval
                HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
            }

            if ((LMIC.opmode & OP_TXRXPEND) == 0) {
				printf("[INFO] Radio is free. Forcing immediate uplink...\n");
				os_clearCallback(&sendjob);
				os_setCallback(&sendjob, do_send);
			} else {
				// If they press the button while a transmission is already in the air,
				// we don't crash the radio. The emergency byte will just get picked up
				// naturally on the next scheduled interval!
				printf("[WARN] Radio is currently busy. Emergency state saved for next window.\n");
			}
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "tim.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include "screen.h"
#include "keyboard.h"

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

/* USER CODE BEGIN PV */
uint8_t columnNumber = 0;
uint8_t current_row = 0;

int screenColumn = 0;
int screenRow = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_USB_OTG_FS_HCD_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  // Turn on Power Switch
  HAL_GPIO_WritePin(Power_Switch_GPIO_Port, Power_Switch_Pin, SET);

  // Initialize LCD screen
  Lcd_PortType ports[] = {
	LCD_D4_GPIO_Port, LCD_D5_GPIO_Port, LCD_D6_GPIO_Port, LCD_D7_GPIO_Port
	};

  Lcd_PinType pins[] = {LCD_D4_Pin, LCD_D5_Pin, LCD_D6_Pin, LCD_D7_Pin};

  // Create LCD handle
  Lcd_HandleTypeDef lcd = Lcd_create(ports, pins, LCD_RS_GPIO_Port, LCD_RS_Pin, LCD_E_GPIO_Port, LCD_E_Pin, LCD_4_BIT_MODE);

  // initialize LCD
  Lcd_init(&lcd);

  // start timer
  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
	  Error_Handler();
  }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(Y2_GPIO_Port, Y2_Pin, GPIO_PIN_RESET);
      for (columnNumber = 0; columnNumber < 11; columnNumber++)  // Cycle through all columns
      {
          setMuxChannel(columnNumber);  // Set the multiplexer to the current column

          // Check if a key is pressed in the current column
          if (readMuxInput() == 0)  // A 0 indicates a key press in the current column
          {
              //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);  // Toggle an LED to show key press detected
              HAL_Delay(5); // Debouncing delay

              // Now scan through the rows to detect the correct row
              for (current_row = 0; current_row < 3; current_row++)
              {
                  // Drive the current row low and others high
                  HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, (current_row == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET);
                  HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, (current_row == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
                  HAL_GPIO_WritePin(Y2_GPIO_Port, Y2_Pin, (current_row == 2) ? GPIO_PIN_RESET : GPIO_PIN_SET);

                  // Check if key press still detected
                  if (readMuxInput() == 0)
                  {
                      char key = handleKeyPress(current_row, columnNumber);  // Get key from row/column
                      char keyString[2] = { key, '\0' };  // Convert to string for LCD display

                      Lcd_cursor(&lcd, 0, 0);  // Set cursor to top-left corner of the display
                      Lcd_string(&lcd, keyString);  // Print key to LCD

                      HAL_Delay(5);  // Short delay so the output is visible
                      break;  // Exit the loop after detecting the key press
                  }
              }
          }
      }
  }

//  while (1)
//  {
//	  // Set the correct multiplexer channel for the current column
//	  setMuxChannel(columnNumber);
//
//	  // Check if a key is pressed in the current column
//	  if (readMuxInput() == 0)
//	  {
//		  HAL_Delay(100); // Debouncing delay
//
//		  // Get the corresponding key press
//		  char key = handleKeyPress(current_row, columnNumber);
//		  char keyString[2] = { key, '\0' }; // Convert to string for LCD display
//
//		  HAL_Delay(50); // Short delay to allow the character to register
//
//		  // Display the key on the LCD
//		  Lcd_string(&lcd, keyString);
//
//		  // Move to the next position on the screen
//		  if (screenColumn < 16)  // 16 characters per row
//		  {
//			  Lcd_cursor(&lcd, screenRow, screenColumn); // Move cursor
//			  screenColumn++;
//		  }
//		  else
//		  {
//			  screenRow++;
//			  screenColumn = 0; // Reset to column 0
//
//			  // If both rows are filled, clear the screen
//			  if (screenRow >= 2)
//			  {
//				  Lcd_clear(&lcd);    // Clear display
//				  screenRow = 0;      // Reset to first row
//			  }
//
//			  Lcd_cursor(&lcd, screenRow, screenColumn);  // Move cursor to new row
//		  }
//	  }
//
//	  // Scan through all columns after checking the current one
//	  columnNumber = (columnNumber + 1) % 11;
//	  current_row = (current_row + 1) % 3;  // Move to the next row in a cyclic manner
//  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) // 4kHz clock
{
	if (htim -> Instance == TIM2) // once timer counts up
	{
		// Reset all rows to high
		HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, SET);
		HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, SET);
		HAL_GPIO_WritePin(Y2_GPIO_Port, Y2_Pin, SET);

		// Drive the current row low
		switch (current_row)
		{
		case 0:
			HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, RESET);
			break;
		case 1:
			HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(Y2_GPIO_Port, Y2_Pin, RESET);
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

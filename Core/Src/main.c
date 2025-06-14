/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "fatfs_sd_card.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint16_t Timer1, Timer2;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */
void process_SD_card( void );
void set_sd_cs_pin(uint16_t pin);
void SD_CS_Select(uint16_t cs_pin);
void SD_CS_Deselect(uint16_t cs_pin);
void write_to_sd_card(const char* filename, const char* data, uint16_t cs_pin);
void read_from_sd_card(const char* filename, char* buffer, int size, uint16_t cs_pin);
void write_to_both_sd_cards(const char* filename, const char* data);
void read_from_both_sd_cards(const char* filename);
void write_to_sd(const char* filename, const char* data);
void read_from_sd(const char* filename, char* buffer, int size);
void test_sd_card_connection(uint16_t cs_pin);

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
  MX_FATFS_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  printf("=== Dual SD Card Utility Functions Test ===\r\n");
  printf("SD Card 1: CS Pin B12\r\n");
  printf("SD Card 2: CS Pin B2\r\n\n");
  
  // First, test both SD card connections
  printf("=== Initial SD Card Connection Tests ===\r\n");
  test_sd_card_connection(GPIO_PIN_12);  // Test SD Card 1
  test_sd_card_connection(GPIO_PIN_2);   // Test SD Card 2
  
  // Test 1: Write configuration to both SD cards
  printf("Test 1: Writing configuration files...\r\n");
  write_to_both_sd_cards("config.txt", "wifi_ssid=MyNetwork\nwifi_pass=123456\ndevice_id=STM32_001\n");
  
  // Test 2: Write sensor data to both SD cards
  printf("Test 2: Writing sensor data...\r\n");
  char sensor_data[100];
  sprintf(sensor_data, "timestamp=%ld,temp=25.6,humidity=60.2\n", HAL_GetTick());
  write_to_both_sd_cards("sensor.log", sensor_data);
  
  // Test 3: Read back the files from both SD cards
  printf("Test 3: Reading back files...\r\n");
  read_from_both_sd_cards("config.txt");
  read_from_both_sd_cards("sensor.log");
  
  // Test 4: Multiple writes to both cards (append simulation)
  printf("Test 4: Multiple log entries...\r\n");
  write_to_both_sd_cards("test_log.txt", "Log Entry 1: System Started\n");
  HAL_Delay(1000);
  write_to_both_sd_cards("test_log.txt", "Log Entry 2: All systems operational\n");
  
  read_from_both_sd_cards("test_log.txt");
  
  // Test 5: System settings to both cards
  printf("Test 5: Writing system settings...\r\n");
  write_to_both_sd_cards("settings.ini", "baudrate=115200\ntimeout=5000\nversion=1.0\n");
  
  read_from_both_sd_cards("settings.ini");
  
  // Test 6: Individual card operations
  printf("Test 6: Individual card operations...\r\n");
  
  printf("Writing specific data to SD Card 1 only...\r\n");
  write_to_sd_card("card1_specific.txt", "This file is only on SD Card 1 (CS: B12)\n", GPIO_PIN_12);
  
  printf("Writing specific data to SD Card 2 only...\r\n");
  write_to_sd_card("card2_specific.txt", "This file is only on SD Card 2 (CS: B2)\n", GPIO_PIN_2);
  
  printf("Reading specific files...\r\n");
  char buffer[200];
  read_from_sd_card("card1_specific.txt", buffer, sizeof(buffer), GPIO_PIN_12);
  read_from_sd_card("card2_specific.txt", buffer, sizeof(buffer), GPIO_PIN_2);
  
  printf("\n=== All Dual SD Card Tests Completed Successfully! ===\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB2 PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* Configure PB14 (MISO) with pull-up for better SD card communication */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  This function is executed to decrement timers
  * @param  None
  * @retval None
  */
void HAL_SYSTICK_Callback(void)
{
  if(Timer1 > 0) Timer1--;
  if(Timer2 > 0) Timer2--;
}

/**
  * @brief  Select CS pin for SD card
  * @param  cs_pin: GPIO pin number (GPIO_PIN_12 or GPIO_PIN_2)
  * @retval None
  */
void SD_CS_Select(uint16_t cs_pin)
{
  HAL_GPIO_WritePin(GPIOB, cs_pin, GPIO_PIN_RESET);
  HAL_Delay(1);
}

/**
  * @brief  Deselect CS pin for SD card
  * @param  cs_pin: GPIO pin number (GPIO_PIN_12 or GPIO_PIN_2)
  * @retval None
  */
void SD_CS_Deselect(uint16_t cs_pin)
{
  HAL_GPIO_WritePin(GPIOB, cs_pin, GPIO_PIN_SET);
  HAL_Delay(1);
}

/**
  * @brief  Write data to specific SD card with full reset
  * @param  filename: Name of the file to write
  * @param  data: Data string to write
  * @param  cs_pin: CS pin for SD card selection (GPIO_PIN_12 or GPIO_PIN_2)
  * @retval None
  */
void write_to_sd_card(const char* filename, const char* data, uint16_t cs_pin) 
{
    FIL fil;
    UINT bw;
    FRESULT fres;
    
    // Ensure both CS pins are high (deselected) first
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_Delay(500);  // Longer delay for complete stabilization
    
    // Set the CS pin for this operation
    set_sd_cs_pin(cs_pin);
    
    // Additional delay after CS pin change
    HAL_Delay(300);
    
    // Force SD disk re-initialization
    SD_disk_initialize(0);
    HAL_Delay(200);
    
    // Mount SD card
    FATFS FatFs;
    fres = f_mount(&FatFs, "", 1);
    if (fres != FR_OK) {
        printf("SD Card %s Mount Error: %d\r\n", (cs_pin == GPIO_PIN_12) ? "1 (B12)" : "2 (B2)", fres);
        
        // Try one more time with re-initialization
        HAL_Delay(500);
        SD_disk_initialize(0);
        HAL_Delay(200);
        fres = f_mount(&FatFs, "", 1);
        if (fres != FR_OK) {
            printf("SD Card %s Mount Error (retry): %d\r\n", (cs_pin == GPIO_PIN_12) ? "1 (B12)" : "2 (B2)", fres);
            return;
        }
    }
    
    // Open/Create file for writing
    fres = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (fres == FR_OK) {
        fres = f_write(&fil, data, strlen(data), &bw);
        if (fres == FR_OK) {
            f_sync(&fil);  // Ensure data is written
            printf("SD Card %s - File written: %s (%u bytes)\r\n", 
                   (cs_pin == GPIO_PIN_12) ? "1 (B12)" : "2 (B2)", filename, bw);
        } else {
            printf("SD Card %s - Write error: %d\r\n", 
                   (cs_pin == GPIO_PIN_12) ? "1 (B12)" : "2 (B2)", fres);
        }
        f_close(&fil);
    } else {
        printf("SD Card %s - File create error: %d\r\n", 
               (cs_pin == GPIO_PIN_12) ? "1 (B12)" : "2 (B2)", fres);
    }
    
    // Unmount SD card
    f_mount(NULL, "", 0);
    
    // Final delay before next operation
    HAL_Delay(200);
}

/**
  * @brief  Read data from specific SD card with full reset
  * @param  filename: Name of the file to read
  * @param  buffer: Buffer to store read data
  * @param  size: Buffer size
  * @param  cs_pin: CS pin for SD card selection (GPIO_PIN_12 or GPIO_PIN_2)
  * @retval None
  */
void read_from_sd_card(const char* filename, char* buffer, int size, uint16_t cs_pin) 
{
    FIL fil;
    FRESULT fres;
    
    // Ensure both CS pins are high (deselected) first
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_Delay(500);  // Longer delay for complete stabilization
    
    // Set the CS pin for this operation
    set_sd_cs_pin(cs_pin);
    
    // Additional delay after CS pin change
    HAL_Delay(300);
    
    // Force SD disk re-initialization
    SD_disk_initialize(0);
    HAL_Delay(200);
    
    // Mount SD card
    FATFS FatFs;
    fres = f_mount(&FatFs, "", 1);
    if (fres != FR_OK) {
        printf("SD Card %s Mount Error: %d\r\n", (cs_pin == GPIO_PIN_12) ? "1 (B12)" : "2 (B2)", fres);
        
        // Try one more time with re-initialization
        HAL_Delay(500);
        SD_disk_initialize(0);
        HAL_Delay(200);
        fres = f_mount(&FatFs, "", 1);
        if (fres != FR_OK) {
            printf("SD Card %s Mount Error (retry): %d\r\n", (cs_pin == GPIO_PIN_12) ? "1 (B12)" : "2 (B2)", fres);
            buffer[0] = '\0';  // Empty string if error
            return;
        }
    }
    
    // Small delay for SD card stability
    HAL_Delay(100);
    
    // Open file for reading
    fres = f_open(&fil, filename, FA_READ);
    if (fres == FR_OK) {
        f_gets(buffer, size, &fil);
        printf("SD Card %s - File read: %s - Content: %s\r\n", 
               (cs_pin == GPIO_PIN_12) ? "1 (B12)" : "2 (B2)", filename, buffer);
        f_close(&fil);
    } else {
        printf("SD Card %s - File read error: %d\r\n", 
               (cs_pin == GPIO_PIN_12) ? "1 (B12)" : "2 (B2)", fres);
        buffer[0] = '\0';  // Empty string if error
    }
    
    // Unmount SD card
    f_mount(NULL, "", 0);
    
    // Final delay before next operation
    HAL_Delay(200);
}

/**
  * @brief  Write data to both SD cards
  * @param  filename: Name of the file to write
  * @param  data: Data string to write
  * @retval None
  */
void write_to_both_sd_cards(const char* filename, const char* data)
{
    printf("\n=== Writing to both SD cards ===\r\n");
    
    // Write to first SD card (CS pin B12)
    printf("Writing to SD Card 1 (CS: B12)...\r\n");
    write_to_sd_card(filename, data, GPIO_PIN_12);
    
    // Small delay between operations
    HAL_Delay(500);
    
    // Write to second SD card (CS pin B2)
    printf("Writing to SD Card 2 (CS: B2)...\r\n");
    write_to_sd_card(filename, data, GPIO_PIN_2);
    
    printf("=== Both SD cards written ===\r\n\n");
}

/**
  * @brief  Read data from both SD cards
  * @param  filename: Name of the file to read
  * @retval None
  */
void read_from_both_sd_cards(const char* filename)
{
    char buffer1[200], buffer2[200];
    
    printf("\n=== Reading from both SD cards ===\r\n");
    
    // Read from first SD card (CS pin B12)
    printf("Reading from SD Card 1 (CS: B12)...\r\n");
    read_from_sd_card(filename, buffer1, sizeof(buffer1), GPIO_PIN_12);
    
    // Small delay between operations
    HAL_Delay(500);
    
    // Read from second SD card (CS pin B2)
    printf("Reading from SD Card 2 (CS: B2)...\r\n");
    read_from_sd_card(filename, buffer2, sizeof(buffer2), GPIO_PIN_2);
    
    printf("=== Both SD cards read ===\r\n\n");
}

/**
  * @brief  Write data to SD card file
  * @param  filename: Name of the file to write
  * @param  data: Data string to write
  * @retval None
  */
void write_to_sd(const char* filename, const char* data) 
{
    FIL fil;
    UINT bw;
    FRESULT fres;
    
    // Mount SD card
    FATFS FatFs;
    fres = f_mount(&FatFs, "", 1);
    if (fres != FR_OK) {
        printf("SD Mount Error: %d\r\n", fres);
        return;
    }
    
    // Open/Create file for writing
    fres = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (fres == FR_OK) {
        fres = f_write(&fil, data, strlen(data), &bw);
        if (fres == FR_OK) {
            f_sync(&fil);  // Ensure data is written
            printf("File written: %s (%u bytes)\r\n", filename, bw);
        } else {
            printf("Write error: %d\r\n", fres);
        }
        f_close(&fil);
    } else {
        printf("File create error: %d\r\n", fres);
    }
    
    // Unmount SD card
    f_mount(NULL, "", 0);
}

/**
  * @brief  Read data from SD card file
  * @param  filename: Name of the file to read
  * @param  buffer: Buffer to store read data
  * @param  size: Buffer size
  * @retval None
  */
void read_from_sd(const char* filename, char* buffer, int size) 
{
    FIL fil;
    FRESULT fres;
    
    // Mount SD card
    FATFS FatFs;
    fres = f_mount(&FatFs, "", 1);
    if (fres != FR_OK) {
        printf("SD Mount Error: %d\r\n", fres);
        return;
    }
    
    // Small delay for SD card stability
    HAL_Delay(100);
    
    // Open file for reading
    fres = f_open(&fil, filename, FA_READ);
    if (fres == FR_OK) {
        f_gets(buffer, size, &fil);
        printf("File read: %s - Content: %s\r\n", filename, buffer);
        f_close(&fil);
    } else {
        printf("File read error: %d\r\n", fres);
        buffer[0] = '\0';  // Empty string if error
    }
    
    // Unmount SD card
    f_mount(NULL, "", 0);
}

#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the UART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

  return ch;
}

/**
  * @brief SD Card Operation
  * @param none
  * @retval none
  */
void process_SD_card( void )
{
  FATFS       FatFs;                //Fatfs handle
  FIL         fil;                  //File handle
  FRESULT     fres;                 //Result after operations
  char        buf[100];

  do
  {
    //Mount the SD Card
    fres = f_mount(&FatFs, "", 1);    //1=mount now
    if (fres != FR_OK)
    {
      printf("No SD Card found : (%i)\r\n", fres);
      break;
    }
    printf("SD Card Mounted Successfully!!!\r\n");

    //Read the SD Card Total size and Free Size
    FATFS *pfs;
    DWORD fre_clust;
    uint32_t totalSpace, freeSpace;

    f_getfree("", &fre_clust, &pfs);
    totalSpace = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
    freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);

    printf("TotalSpace : %lu bytes, FreeSpace = %lu bytes\n", totalSpace, freeSpace);

    //Create and write file
    fres = f_open(&fil, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if(fres != FR_OK)
    {
      printf("File creation Error : (%i)\r\n", fres);
      break;
    }

    printf("Writing data!!!\r\n");
    
    // Write data using f_write instead of f_puts for better control
    const char* text = "Hello World!\r\n";
    UINT bytes_written;
    fres = f_write(&fil, text, strlen(text), &bytes_written);
    if(fres != FR_OK) {
      printf("f_write Error : (%i)\r\n", fres);
      f_close(&fil);
      break;
    }
    printf("Bytes written: %u\r\n", bytes_written);
    
    // Sync to ensure data is written to SD card
    fres = f_sync(&fil);
    if(fres != FR_OK) {
      printf("f_sync Error : (%i)\r\n", fres);
      f_close(&fil);
      break;
    }
    
    // Close file with error checking
    fres = f_close(&fil);
    if(fres != FR_OK) {
      printf("f_close Error : (%i)\r\n", fres);
      break;
    }
    
    printf("File written and closed successfully!\r\n");
    
    // Small delay to ensure SD card operations complete
    HAL_Delay(200);
    
    printf("Skipping f_stat, going directly to file read...\r\n");
    
    // Try to read the file directly without f_stat
    fres = f_open(&fil, "test.txt", FA_READ);
    printf("f_open for read result: %d\r\n", fres);
    
    if(fres == FR_OK)
    {
      printf("SUCCESS: File opened for reading!\r\n");
      f_gets(buf, sizeof(buf), &fil);
      printf("Read Data: %s\r\n", buf);
      f_close(&fil);
      printf("File closed after reading!\r\n");
    }
    else
    {
      printf("FAILED: Cannot open file for reading! Error: %d\r\n", fres);
    }
    
    printf("Test completed!\r\n");
    
  } while( false );

  //Unmount the drive
  f_mount(NULL, "", 0);
  printf("SD Card Unmounted Successfully!!!\r\n");
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

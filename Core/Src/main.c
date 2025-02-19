/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 主程序入口（基于 v48 完善版本）
  ******************************************************************************
  * @attention
  *
  * 版权所有 (c) 2024 STMicroelectronics。
  * 保留所有权利。
  *
  * 本软件根据 LICENSE 文件中的许可条款分发，
  * 若无 LICENSE 文件，则按“原样”提供。
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dfplayer.h"
#include "bluetooth.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* 无 */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* 用户私有定义，可在此添加所需的宏或常量 */
#define TIME_UNIT 100
#define SENSOR_TIMEOUT 1000      // 添加SENSOR_TIMEOUT定义，根据实际需要设置值
#define SENSOR_TIMEOUT_DISP 1000  // 根据实际需要定义的值
#define COMMAND_BUFFER_SIZE 256   // 修改1: 定义 COMMAND_BUFFER_SIZE 为256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* 用户私有宏定义 */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* 用户私有变量 */
/* 外部变量引用 */
extern uint8_t cur_screen, need_update_menu;
extern uint8_t is_next_day;
extern int upcoming_schedule_pos;
extern SCHEDULE schedule_list[10];
extern uint8_t content[10][21];
extern RTC_Time c_time;
extern int upcoming_time;
/* 内部计数及状态变量 */
int time_update_cnt = 0;
int medicine_notify = 0, medicine_notify_cnt = 0;
int type_a_cnt = 0, type_b_cnt = 0;
unsigned long last_interrupt_cnt_a = 0, last_interrupt_cnt_b = 0;
int sensor_tmout = 0, sensor_tmout_cnt = 0, sensor_tmout_cnt2 = 0;
int upcoming_delay = 0;
uint8_t rx_data[2];
uint8_t command_buffer[COMMAND_BUFFER_SIZE], command_buffer_cnt = 0; // 修改2: 初始化 command_buffer_cnt 为 0
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
/* 私有函数原型声明 */
void time_update(void);
void stepper_control(void);
void process_command(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* 此处可以添加其他全局初始化代码 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* 用户初始化代码 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* 用户初始化代码 */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* 用户系统级初始化 */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  
  /* 初始化重要IO口状态 */
  HAL_GPIO_WritePin(TYPEA_GPIO_Port, TYPEA_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TYPEB_GPIO_Port, TYPEB_Pin, GPIO_PIN_RESET);
  HAL_Delay(500);     // I2C初始化延时
  
  /* 初始化 DS3231 实时时钟 */
  DS3231_Init(&hi2c1);
#if !RTC_EMUL
  // 若RTC年份不正确则进行更新
  if (DS3231_GetYear() != 24) {
      DS3231_SetYear(24);
  }
#endif

  /* 初始化 LCD 显示模块 */
  lcd_init();
  lcd_clear_display();
  lcd_gotoxy(1, 1);
  lcd_send_string("Initialize...");

  /* 装载计划列表 */
  load_schedule();
  upcoming_time = 999999;

  /* 设置并刷新菜单显示 */
  menu_set_content();
  menu_update();

  /* 启动 UART 中断接收 */
  HAL_UART_Receive_IT(&huart2, rx_data, 1);

  HAL_Delay(1000);

  /* 初始化 DFPlayer 语音模块 */
  DFPlayer_Init();

  /* 初始化蓝牙模块 */
  Bluetooth_Init();
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      /* 按键检测及刷新菜单 */
      check_button();
      menu_update();
      /* 更新时间显示及调度 */
      time_update();
      /* 控制步进电机进行提醒操作 */
      stepper_control();
      
      /* 延时，单位为TIME_UNIT */
      HAL_Delay(TIME_UNIT);
  }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */
  /* 用户代码 I2C1 初始化前置代码 */
  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */
  /* 用户代码 I2C1 初始化预处理 */
  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */
  /* 用户代码 I2C1 初始化后处理 */
  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */
  /* 用户代码 USART2 初始化前置 */
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */
  /* 用户代码 USART2 初始化预处理 */
  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  /* 用户代码 USART2 初始化后处理 */
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
  /* 用户代码 GPIO 初始化前置 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BUZZ_Pin|TYPEB_Pin|TYPEA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BT_BK_Pin */
  GPIO_InitStruct.Pin = BT_BK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BT_BK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BUZZ_Pin TYPEB_Pin TYPEA_Pin */
  GPIO_InitStruct.Pin = BUZZ_Pin|TYPEB_Pin|TYPEA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : TYPEA_CNT_INTR_Pin TYPEB_CNT_INTR_Pin */
  GPIO_InitStruct.Pin = TYPEA_CNT_INTR_Pin|TYPEB_CNT_INTR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BT_SL_Pin BT_DN_Pin BT_UP_Pin */
  GPIO_InitStruct.Pin = BT_SL_Pin|BT_DN_Pin|BT_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  /* 用户代码 GPIO 初始化后处理 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  更新时间函数，1秒更新一次
  */
void time_update(void)
{
    time_update_cnt++;
    HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, (medicine_notify ? GPIO_PIN_SET : GPIO_PIN_RESET));
    if (time_update_cnt == 1000 / TIME_UNIT)   // 1秒标识
    {
        time_update_cnt = 0;
#if RTC_EMUL
        c_time.seconds++;
        if(c_time.seconds == 60) {
            c_time.seconds = 0;
            c_time.minutes++;
            if(c_time.minutes == 60) {
                c_time.minutes = 0;
                c_time.hours++;
                if(c_time.hours == 24) {
                    c_time.hours = 0;
                }
            }
        }
#else
        DS3231_GetFullDateTime(&c_time);
#endif
        /* 处理传感器超时计数 */
        if(sensor_tmout_cnt != 0)
        {
            sensor_tmout_cnt--;
            if(sensor_tmout_cnt == 0)
            {
                if(sensor_tmout != 0)
                {
                    sensor_tmout_cnt2 = SENSOR_TIMEOUT_DISP;
                    medicine_notify = 0;
                    medicine_notify_cnt = 0;
                    /* 修改4: 确保 type_a_cnt 与 type_b_cnt 已在变量声明时初始化 */
                    type_a_cnt = 0;
                    type_b_cnt = 0;
                    HAL_GPIO_WritePin(TYPEA_GPIO_Port, TYPEA_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(TYPEB_GPIO_Port, TYPEB_Pin, GPIO_PIN_RESET);
                }
            }
        }
        if(sensor_tmout_cnt2 != 0)
        {
            sensor_tmout_cnt2--;  // 修改5: sensor_tmout_cnt2 已初始化，无需特殊处理
        }
        /* 处理提醒计数 */
        if(medicine_notify_cnt != 0)
        {
            medicine_notify_cnt--;
            if(medicine_notify_cnt == 0)
            {
                medicine_notify = 0;
            }
        }
        /* 更新即将触发的计划延时 */
        if(upcoming_delay != 0)
        {
            upcoming_delay--;
            if(upcoming_delay == 0)
            {
                update_upcoming_to_esp(current_schedule);
            }
        }
        /* 判断是否该触发语音提醒 */
        if (medicine_notify == 0)
        {
            if (c_time.hours == upcoming_time / 60 && c_time.minutes == upcoming_time % 60)
            {
                medicine_notify = 1;
                type_a_cnt = schedule_list[upcoming_schedule_pos].type_a;
                type_b_cnt = schedule_list[upcoming_schedule_pos].type_b;
                /* 置位传感器超时标识：bit0 - 类型A, bit1 - 类型B */
                sensor_tmout = 0x03;
                sensor_tmout_cnt = SENSOR_TIMEOUT;
                sensor_tmout_cnt2 = 0;

                /* 更新当前计划，并从计划列表中移除该计划 */
                current_schedule = schedule_list[upcoming_schedule_pos];
                upcoming_delay = 3;
                schedule_remove(upcoming_schedule_pos);
                upcoming_time = 999999;
                upcoming_schedule_pos = -1;
                store_schedule();
                update_schedulelist_to_esp();

                /* 修改6: 调用 DFPlayer 播放语音文件函数，应确保该函数已正确定义 */
                DFPlayer_Play(1);
            }
        }
        /* 若已错过触发时间，查找下一个计划 */
        if ((convert_to_minute(c_time.hours, c_time.minutes) - upcoming_time >= 1 && is_next_day == 0)
                || (upcoming_time == 999999))
        {
            find_upcoming_schedule();
        }
        /* 刷新菜单内容 */
        menu_set_content();
    }
}

/**
  * @brief  控制步进电机进行语音提示效果
  */
void stepper_control(void)
{
    if(type_a_cnt > 0)
    {
        HAL_GPIO_TogglePin(TYPEA_GPIO_Port, TYPEA_Pin);
    }
    if(type_b_cnt > 0)
    {
        HAL_GPIO_TogglePin(TYPEB_GPIO_Port, TYPEB_Pin);
    }
}

/**
  * @brief  外部中断回调函数
  * @param  GPIO_Pin：中断触发的引脚编号
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    unsigned long interrupt_time = HAL_GetTick();

    if(GPIO_Pin == GPIO_PIN_14)   // 类型A外部中断
    {
        if((interrupt_time - last_interrupt_cnt_a > 200) && (type_a_cnt > 0))
        {
            type_a_cnt--;
            sensor_tmout &= ~0x01;  // 清除类型A超时标志
            if(type_a_cnt == 0)
            {
                HAL_GPIO_WritePin(TYPEA_GPIO_Port, TYPEA_Pin, GPIO_PIN_RESET);
                if(type_b_cnt == 0)
                {
                    medicine_notify_cnt = 120;  // 2分钟计数
                }
            }
        }
        last_interrupt_cnt_a = interrupt_time;
    }
    else if(GPIO_Pin == GPIO_PIN_15)  // 类型B外部中断
    {
        if((interrupt_time - last_interrupt_cnt_b > 200) && (type_b_cnt > 0))
        {
            type_b_cnt--;
            sensor_tmout &= ~0x02;  // 清除类型B超时标志
            if(type_b_cnt == 0)
            {
                HAL_GPIO_WritePin(TYPEB_GPIO_Port, TYPEB_Pin, GPIO_PIN_RESET);
                if(type_a_cnt == 0)
                {
                    medicine_notify_cnt = 120;  // 2分钟计数
                }
            }
        }
        last_interrupt_cnt_b = interrupt_time;
    }
}

/**
  * @brief  UART 接收中断回调函数
  * @param  huart：接收中断的 UART 句柄
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART2)
    {
        // 接收到的数据存入命令缓冲区
        command_buffer[command_buffer_cnt] = rx_data[0];
        command_buffer_cnt++;
        if(command_buffer_cnt >= COMMAND_BUFFER_SIZE)
        {
            command_buffer_cnt = 0;
        }
        /* 处理接收到的指令 */
        process_command();
        /* 重新启动 UART 接收中断 */
        HAL_UART_Receive_IT(&huart2, rx_data, 1);
    }
}

/**
  * @brief  处理命令数据，根据特定的命令执行操作（设置RTC、更新计划、WiFi连接反馈）
  */
void process_command(void)
{
    if(command_buffer_cnt >= 4 &&
       command_buffer[command_buffer_cnt - 2] == 0x84 &&
       command_buffer[command_buffer_cnt - 1] == 0xF1)
    {
        /* 查找命令起始位（标识符 0x84 0xF0） */
        int start_pos;
        for(start_pos = 0; start_pos < command_buffer_cnt - 2; start_pos++)
        {
            if(command_buffer[start_pos] == 0x84 && command_buffer[start_pos + 1] == 0xF0)
            {
                break;
            }
        }
        uint8_t* buff = command_buffer + start_pos;
        if(buff[2] == 0x81)  // 设置 RTC 时间命令
        {
#if RTC_EMUL
            c_time.hours = buff[3];
            c_time.minutes = buff[4];
            c_time.seconds = buff[5];
            c_time.day = buff[6];
            c_time.month = buff[7];
            c_time.year = buff[8];
#else
            DS3231_SetFullTime(buff[3], buff[4], buff[5]);
            DS3231_SetDate(buff[6]);
            DS3231_SetMonth(buff[7]);
            DS3231_SetYear(buff[8]);
#endif
        }
        else if(buff[2] == 0x82)  // 设置计划列表命令
        {
            schedule_size = buff[3];
            for(int i = 0; i < schedule_size; i++)
            {
                schedule_list[i].hour = buff[3 * i + 4];
                schedule_list[i].minute = buff[3 * i + 5];
                /* 下发的数据中，高4位为 type_b，低4位为 type_a */
                schedule_list[i].type_a = buff[3 * i + 6] & 0x0F;
                schedule_list[i].type_b = (buff[3 * i + 6] >> 4) & 0x0F;
            }
            store_schedule();
            find_upcoming_schedule();
        }
        else if(buff[2] == 0x83 && buff[3] == 0x80)  // WiFi连接成功反馈命令
        {
            wifi_connect = 1;
            menu_set_content();
        }
        /* 重置命令缓冲区计数 */
        command_buffer_cnt = 0;
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

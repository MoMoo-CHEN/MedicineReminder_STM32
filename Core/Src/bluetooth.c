#include "bluetooth.h"
#include "dfplayer.h"

extern UART_HandleTypeDef huart2;

/* 蓝牙接收缓冲区及索引 */
uint8_t bluetooth_rx_buffer[100];
uint16_t bluetooth_rx_index = 0;

/**
  * @brief  蓝牙初始化函数，启动 UART 中断接收
  * @retval None
  */
void Bluetooth_Init(void)
{
    /* 启动 UART 接收中断，由 HAL 库调用 HAL_UART_RxCpltCallback */
    HAL_UART_Receive_IT(&huart2, bluetooth_rx_buffer, 1);
}

/**
  * @brief  蓝牙数据处理函数
  * @param  data 收到的数据缓冲区
  * @param  length 数据长度
  * @retval None
  *
  * 说明：此函数用于解析通过蓝牙接收到的命令。
  * 当接收到的命令以 "VOICE" 开头时，将解析后续字符，并利用 DFPlayer 播放相应语音文件。
  */
void Bluetooth_ProcessData(uint8_t *data, uint16_t length)
{
    /* 简单解析：如果数据内容以 "VOICE" 开头，则认为为语音文件更改命令 */
    if (length >= 6 &&
        data[0] == 'V' &&
        data[1] == 'O' &&
        data[2] == 'I' &&
        data[3] == 'C' &&
        data[4] == 'E')
    {
        /* 假设第6个字符为语音文件编号（字符型数字） */
        uint8_t fileNumber = data[5] - '0';
        DFPlayer_Play(fileNumber);
    }
}

/**
  * @brief  UART 接收中断回调函数（蓝牙专用）
  * @param  huart：UART 句柄
  * @retval None
  *
  * 此函数用于接收蓝牙模块通过 UART 发来的数据，
  * 并在检测到换行符时调用 Bluetooth_ProcessData 函数进行数据解析处理。
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART2)
    {
        /* 将接收到的一个字节存入缓冲区 */
        bluetooth_rx_buffer[bluetooth_rx_index++] = bluetooth_rx_buffer[0];

        /* 若检测到换行符或缓冲区已满，则处理数据 */
        if (bluetooth_rx_index >= 100 || bluetooth_rx_buffer[bluetooth_rx_index - 1] == '\n')
        {
            Bluetooth_ProcessData(bluetooth_rx_buffer, bluetooth_rx_index);
            bluetooth_rx_index = 0;
        }
        /* 继续接收下一个字节 */
        HAL_UART_Receive_IT(&huart2, bluetooth_rx_buffer, 1);
    }
}
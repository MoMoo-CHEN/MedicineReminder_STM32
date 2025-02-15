#include "usart.h"

/**
 * @brief 初始化 USART 外设
 * 请根据具体外设配置修改此函数，例如波特率、数据位、停止位等。
 */
void USART_Init(void) {
    // 示例：使用 HAL 库进行初始化的占位代码，请根据实际情况进行修改
    // UART_HandleTypeDef huart;
    // huart.Instance = USART1;
    // huart.Init.BaudRate = 115200;
    // huart.Init.WordLength = UART_WORDLENGTH_8B;
    // huart.Init.StopBits = UART_STOPBITS_1;
    // huart.Init.Parity = UART_PARITY_NONE;
    // huart.Init.Mode = UART_MODE_TX_RX;
    // huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    // huart.Init.OverSampling = UART_OVERSAMPLING_16;
    // if (HAL_UART_Init(&huart) != HAL_OK) {
    //     // 初始化错误处理
    // }
}

/**
 * @brief 发送一个字符
 */
void USART_SendChar(char c) {
    // 示例：使用 HAL 库发送一个字符的占位代码
    // HAL_UART_Transmit(&huart, (uint8_t *)&c, 1, HAL_MAX_DELAY);

    // 若未使用 HAL 库，可使用自定义实现：
    // 等待发送寄存器空闲，然后写入数据
}

/**
 * @brief 接收一个字符
 */
char USART_ReceiveChar(void) {
    char c = 0;
    // 示例：使用 HAL 库接收一个字符的占位代码
    // HAL_UART_Receive(&huart, (uint8_t *)&c, 1, HAL_MAX_DELAY);
    
    // 若未使用 HAL 库，可使用自定义实现：
    // 等待接收数据并返回
    return c;
}

/**
 * @brief 发送一个字符串
 */
void USART_SendString(const char* str) {
    while (*str) {
        USART_SendChar(*str++);
    }
}

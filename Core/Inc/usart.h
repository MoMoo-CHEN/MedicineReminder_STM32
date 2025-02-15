#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"  // 根据你的目标 MCU 系列修改此引用

/**
 * @brief 初始化 USART 外设
 */
void USART_Init(void);

/**
 * @brief 通过 USART 发送一个字符
 * @param c 要发送的字符
 */
void USART_SendChar(char c);

/**
 * @brief 接收一个字符
 * @return 接收到的字符
 */
char USART_ReceiveChar(void);

/**
 * @brief 通过 USART 发送一个字符串
 * @param str 指向字符串的指针
 */
void USART_SendString(const char* str);

#ifdef __cplusplus
}
#endif

#endif  // __USART_H

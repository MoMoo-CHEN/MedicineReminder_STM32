#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "usart.h"

void Bluetooth_Init(void);
void Bluetooth_ProcessData(uint8_t *data, uint16_t length);

#endif /* __BLUETOOTH_H */
#ifndef __DFPLAYER_H
#define __DFPLAYER_H

#include "usart.h"

void DFPlayer_SendCommand(uint8_t command, uint16_t parameter);
void DFPlayer_Play(uint8_t fileNumber);
void DFPlayer_Init();

#endif /* __DFPLAYER_H */
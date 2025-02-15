#include "dfplayer.h"

void DFPlayer_SendCommand(uint8_t command, uint16_t parameter) {
    uint8_t cmd[10] = {0x7E, 0xFF, 0x06, command, 0x00, (uint8_t)(parameter >> 8), (uint8_t)(parameter & 0xFF), 0x00, 0x00, 0xEF};
    uint16_t checksum = 0;
    for (int i = 1; i < 7; i++) {
        checksum += cmd[i];
    }
    checksum = -checksum;
    cmd[7] = (uint8_t)(checksum >> 8);
    cmd[8] = (uint8_t)(checksum & 0xFF);
    HAL_UART_Transmit(&huart2, cmd, 10, 100);
}

void DFPlayer_Play(uint8_t fileNumber) {
    DFPlayer_SendCommand(0x03, fileNumber);
}

void DFPlayer_Init() {
    DFPlayer_SendCommand(0x3F, 0x0000); // Reset the module
    HAL_Delay(1000); // Wait for the module to reset
    DFPlayer_SendCommand(0x06, 0x1E); // Set volume (0x1E is volume level)
}
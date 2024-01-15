#include "flash.h"

uint32_t GetSector(uint32_t Address) {
	uint32_t sector;

	if ((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0)) {
		sector = FLASH_SECTOR_0;
	} else if ((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1)) {
		sector = FLASH_SECTOR_1;
	} else if ((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2)) {
		sector = FLASH_SECTOR_2;
	} else if ((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3)) {
		sector = FLASH_SECTOR_3;
	} else if ((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4)) {
		sector = FLASH_SECTOR_4;
	} else if ((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5)) {
		sector = FLASH_SECTOR_5;
	} else if ((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6)) {
		sector = FLASH_SECTOR_6;
	} else if ((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7)) {
		sector = FLASH_SECTOR_7;
	}
	else {
		sector = -1;
	}

	return sector;
}

uint32_t Flash_Write_Data(uint32_t Address, uint32_t *data_t, uint32_t size) {

	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	int i = 0;

//	int numberofwords = (strlen((char*) data_t) / 4) + ((strlen((char*) data_t) % 4) != 0);

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area*/

	uint32_t StartSector = GetSector(Address);
	uint32_t EndAdress = Address + size * 4;
	uint32_t EndSector = GetSector(EndAdress);

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = StartSector;
	EraseInitStruct.NbSectors = EndSector - StartSector + 1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
		/*Error occurred while page erase.*/
		return HAL_FLASH_GetError();
	}

	/* Program the user Flash area word by word*/

	i = 0;
	while (i < size) {
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, data_t[i]) == HAL_OK) {
			Address += 4;
			i++;
		} else {
			/* Error occurred while writing data in Flash memory*/
			return HAL_FLASH_GetError();
		}
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	return 0;
}

void Flash_Read_Data(uint32_t Address, __IO uint32_t *data_t, uint32_t size) {
	uint32_t i = 0;
	while (i < size) {
		*data_t = *(__IO uint32_t*) Address;
		i++;
		Address += 4;
		data_t++;
	}
}

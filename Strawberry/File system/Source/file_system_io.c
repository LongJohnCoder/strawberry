// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "file_system_io.h"
#include "board_sd_card.h"
#include "sd_protocol.h"


//--------------------------------------------------------------------------------------------------//


// Make the physical disk
sd_card card;


//--------------------------------------------------------------------------------------------------//


fatfs_status_t disk_status_fat(uint8_t physical_drive)
{
	uint8_t status = board_sd_card_get_status();
	
	if (status == 1)
	{
		if (card.card_initialized == 1)
		{
			return FATFS_STATUS_OK;
		}
		else
		{
			return FATFS_STATUS_NO_INIT;
		}
	}
	else
	{
		return FATFS_STATUS_NO_DISK;
	}
}


//--------------------------------------------------------------------------------------------------//


fatfs_status_t disk_initialize_fat(uint8_t physical_drive)
{
	if (board_sd_card_get_status() == 0)
	{
		return FATFS_STATUS_NO_DISK;
	}
	else
	{
		uint8_t status = sd_protocol_initialize(&card);
		
		if (status == 1)
		{
			return FATFS_STATUS_OK;
		}
		else 
		{
			return FATFS_STATUS_NO_INIT;
		}
	}
}


//--------------------------------------------------------------------------------------------------//


fatfs_result_t disk_read_fat(uint8_t physical_drive, uint8_t* data, uint32_t sector, uint32_t count)
{
	// First check if the section is supported on the card
	if (sector + count >= card.number_of_blocks)
	{
		return RES_PARERR;
	}
	else
	{
		// The command can be executed on the SD card
		uint8_t status = sd_protocol_read(&card, data, sector, count);
		
		if (status == 0)
		{
			return RES_ERROR;
		}
		else 
		{
			return RES_OK;
		}
	}
}


//--------------------------------------------------------------------------------------------------//


fatfs_result_t disk_write_fat(uint8_t physical_drive, const uint8_t* data, uint32_t sector, uint32_t count)
{
	// First check if the section is supported on the card
	if (sector + count >= card.number_of_blocks)
	{
		return RES_PARERR;
	}
	else
	{
		// The command can be executed on the SD card
		uint8_t status = sd_protocol_write(&card, data, sector, count);
		
		if (status == 0)
		{
			return RES_ERROR;
		}
		else
		{
			return RES_OK;
		}
	}
}


//--------------------------------------------------------------------------------------------------//


fatfs_result_t disk_ioctl(uint8_t physical_drive, uint8_t command, void* data)
{
	switch (command)
	{
		case CTRL_SYNC:
			// Complete pending write processes
			// Since we are currently not using DMA this always returns OK
			return RES_OK;
		case GET_SECTOR_COUNT:
			// Get the number of sectors
			if (card.card_initialized)
			{
				*((uint32_t *)data) = card.number_of_blocks;
				return RES_OK;
			}
			else
			{
				return RES_ERROR;
			}
		
		case GET_SECTOR_SIZE:
			// Return the sector size
			*((uint16_t *)data) = 512;
			return RES_OK;
	}
	return RES_ERROR;
}


//--------------------------------------------------------------------------------------------------//


uint32_t get_fattime(void)
{
	// Not used in this implementation
	return 0;
}


//--------------------------------------------------------------------------------------------------//


void disk_print_info(void)
{
	sd_protocol_print_card_info(&card);
}


//--------------------------------------------------------------------------------------------------//


void disk_print_csd(void)
{
	sd_protocol_print_csd(&card);
}


//--------------------------------------------------------------------------------------------------//
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef DISKIO_H
#define DISKIO_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


// Defines for fatfs_status_t
#define FATFS_STATUS_OK 0x00
#define FATFS_STATUS_NO_INIT 0x01
#define FATFS_STATUS_NO_DISK 0x02
#define FATFS_STATUS_DISK_PROTECTED 0x04


#define STA_NOINIT		0x01
#define STA_NODISK		0x02
#define STA_PROTECT		0x04


// Defines for disk_ioctrl arguments
#define CTRL_SYNC			0
#define GET_SECTOR_COUNT	1
#define GET_SECTOR_SIZE		2
#define GET_BLOCK_SIZE		3
#define CTRL_TRIM			4


typedef uint8_t fatfs_status_t;
typedef fatfs_status_t DSTATUS;


typedef enum {
	RES_OK = 0,
	RES_ERROR,
	RES_WRPRT,
	RES_NOTRDY,
	RES_PARERR
} fatfs_result_t;


//--------------------------------------------------------------------------------------------------//


fatfs_status_t disk_status_fat(uint8_t physical_drive);

fatfs_status_t disk_initialize_fat(uint8_t physical_drive);

fatfs_result_t disk_read_fat(uint8_t physical_drive, uint8_t* data, uint32_t sector, uint32_t count);

fatfs_result_t disk_write_fat(uint8_t physical_drive, const uint8_t* data, uint32_t sector, uint32_t count);

fatfs_result_t disk_ioctl(uint8_t physical_drive, uint8_t command, void* data);

uint32_t get_fattime(void);


//--------------------------------------------------------------------------------------------------//


void disk_print_info(void);

void disk_print_csd(void);


//--------------------------------------------------------------------------------------------------//


#endif
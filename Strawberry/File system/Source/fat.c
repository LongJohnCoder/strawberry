// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "fat.h"
#include "file_system_io.h"
#include "board_serial.h"
#include "board_sd_card.h"

//--------------------------------------------------------------------------------------------------//

uint8_t buffer[512];

static int mem_cmp (const void* dst, const void* src, uint32_t cnt)	/* ZR:same, NZ:different */
{
	const char *d = (const char *)dst, *s = (const char *)src;
	int r = 0;

	do {
		r = *d++ - *s++;
	} while (--cnt && r == 0);

	return r;
}

//--------------------------------------------------------------------------------------------------//

void fat_config(void)
{
	board_sd_card_config();
	for (uint8_t i = 0; i < 10; i++)
	{
		if (disk_initialize(0) == 0)
		{
			break;
		}
	}
	disk_read(0, buffer, 0, 1);
	
	for (uint16_t i = 0; i < 512; i++)
	{
		board_serial_print_hex((char)buffer[i]);
		board_serial_print("\t");
		if ((i != 0) && (((i + 11) % 10) == 0))
		{
			board_serial_print("\n");
		}
	}
	for (uint16_t i = 0; i < 512; i++)
	{
		board_serial_print("%c", buffer[i]);
		if ((i != 0) && (((i + 11) % 10) == 0))
		{
			board_serial_print("\n");
		}
	}
}

//--------------------------------------------------------------------------------------------------//
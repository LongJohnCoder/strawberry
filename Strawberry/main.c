// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "kernel.h"
#include "board_serial.h"
#include "gpio.h"
#include "mutex.h"
#include "dynamic_memory.h"
#include "thread.h"
#include "spinlock.h"
#include "critical_section.h"
#include "fat.h"
#include "file_system_fat.h"
#include "dynamic_memory.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


void runtime_stats(void* arg)
{
	while (1)
	{
		syscall_sleep(1000);
		print_runtime_statistics();
	}
}


//--------------------------------------------------------------------------------------------------//


int main(void)
{
	// This functions starts up the kernel and initializes the basic drivers
	kernel_startup();
	
	board_serial_print("BIOS success\n\n");
	
	// Test the FAT32 file system
	
	struct disk test = {.disk_number = 1};
	
	
	
	board_serial_print("Size of the volume: %d\n", sizeof(struct volume));
	
	// We use dynamic memory so lets check the stats for it
	uint8_t percent = dynamic_memory_get_used_percentage(SRAM);
	board_serial_print_percentage_symbol("Memory: ", percent, 1);
	
	disk_mount(&test);
	
	percent = dynamic_memory_get_used_percentage(SRAM);
	board_serial_print_percentage_symbol("Memory: ", percent, 1);
	
	
	
	struct volume* first_fat = fat_get_first_volume();
	
	first_fat = first_fat->next;
	
	board_serial_print_n(first_fat->label, 11);
	board_serial_print(" (%c:)\n", first_fat->letter);
	
	test_print_directories(first_fat);

	
	while (1);
}


//--------------------------------------------------------------------------------------------------//
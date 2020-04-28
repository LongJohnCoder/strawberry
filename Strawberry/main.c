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
	
	thread_new("Runtime", runtime_stats, NULL, THREAD_PRIORITY_INTERACTIVE, 100);
	thread_new("FAT32", fat_32_thread, NULL, THREAD_PRIORITY_INTERACTIVE, 300);	

	
	kernel_launch();
}


//--------------------------------------------------------------------------------------------------//
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "kernel.h"
#include "board_led.h"
#include "board_serial.h"
#include "gpio.h"
#include "mutex.h"
#include "dynamic_memory.h"
#include "thread.h"


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


void welcome_thread(void* arg)
{
	// Print a happy message to the screen
	board_serial_print("Kernel successfully started\n\n");
}

#include "spinlock.h"
struct spinlock test;
volatile int k = 1001;
void incr(void* arg)
{
	for (uint32_t i = 0; i < 100000000; i++)
	{
		spinlock_aquire(&test);
		k++;
		spinlock_release(&test);
	}
	board_serial_print("Incr: %d\n", k);
}


void decr(void* arg)
{
	for (uint32_t i = 0; i < 100000000; i++)
	{
		spinlock_aquire(&test);
		k--;
		spinlock_release(&test);
	}
	board_serial_print("Decr: %d\n", k);
}


//--------------------------------------------------------------------------------------------------//


int main(void)
{
	// This functions starts up the kernel and initializes the basic drivers
	kernel_startup();
	
	
	// Add some threads for test & debug purposes
	thread_new("blink", blink_thread, NULL, THREAD_PRIORITY_NORMAL, 100);
	
	
	thread_new("decrement", decr, NULL, THREAD_PRIORITY_REAL_TIME, 50);
	thread_s* tmp = thread_new("increment", incr, NULL, THREAD_PRIORITY_REAL_TIME, 50);
	thread_new("welcome", welcome_thread, NULL, THREAD_PRIORITY_REAL_TIME, 50);
	thread_new("runtime", runtime_stats, NULL, THREAD_PRIORITY_NORMAL, 100);
	tmp->ID = 45;
	// Start the kernel
	kernel_launch();
	
	while (1);
}


//--------------------------------------------------------------------------------------------------//
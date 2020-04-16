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


void wav1(void* arg)
{
	gpio_set_pin_function(PIOB, 2, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOB, 2);
	while (1)
	{
		gpio_toogle_pin_value(PIOB, 2);
		syscall_sleep(1);
	}
}


void wav2(void* arg)
{
	gpio_set_pin_function(PIOB, 3, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOB, 3);
	while (1)
	{
		gpio_toogle_pin_value(PIOB, 3);
		syscall_sleep(1);
	}
}


void wav3(void* arg)
{
	gpio_set_pin_function(PIOA, 0, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOA, 0);
	while (1)
	{
		gpio_toogle_pin_value(PIOA, 0);
		syscall_sleep(1);
	}
}

//--------------------------------------------------------------------------------------------------//


int main(void)
{
	// This functions starts up the kernel and initializes the basic drivers
	kernel_startup();
	
	
	// Add some threads for test & debug purposes
	thread_new("welcome", welcome_thread, NULL, THREAD_PRIORITY_REAL_TIME, 50);
	thread_new("runtime", runtime_stats, NULL, THREAD_PRIORITY_NORMAL, 100);
	thread_new("wav1", wav1, NULL, THREAD_PRIORITY_NORMAL, 100);
	thread_new("wav2", wav2, NULL, THREAD_PRIORITY_NORMAL, 100);
	thread_new("wav3", wav3, NULL, THREAD_PRIORITY_NORMAL, 100);

	// Start the kernel
	kernel_launch();
	
	while (1);
}


//--------------------------------------------------------------------------------------------------//
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef CONFIG_H
#define CONFIG_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "board_serial_programming.h"


//--------------------------------------------------------------------------------------------------//


// This is the configuration file for the BlackOS
//
// If this file is changed, the user might need to recompile the entire solution, that is, clean
// the solution and then compile it. Otherwise some variables will noe be updated. 
//
// Due to kernel changes, some of the variables in this config file is deprecated and should not
// be used. 


//--------------------------------------------------------------------------------------------------//


// Flash configuration

// This must be set according to the processor frequency
// For a processor running at 300 MHz, this value should be greater than 10
#define FLASH_NUMBER_OF_WAIT_STATES			10


//--------------------------------------------------------------------------------------------------//


// Dynamic memory core configuration

// This defined the heap alignment in number of bytes
#define DYNAMIC_MEMORY_ALIGN				8

// This defines the minimum allowed block size in bytes
#define DYNAMIC_MEMORY_MINIMUM_BLOCK_SIZE	8

// This specifies if the dynamic memory should be initialized
#define DYNAMIC_MEMORY_FILL					1

// Fill value if the dynamic memory should be initialized
#define DYNAMIC_MEMORY_FILL_VALUE			0x00

// Max name length for a memory section
#define DYNAMIC_MEMORY_SECTION_NAME_SIZE	32


//--------------------------------------------------------------------------------------------------//


// Clock configuration

#define CPU_FREQUENCY						300000000

// This is the usual crystal start up time
#define CLOCK_CRYSTAL_STARTUP_TIME			62

// PLL running at 300MHz
#define CLOCK_PLLA_MULTIPLIER				25


#define CLOCK_PLLA_DIVISION					1


#define CLOCK_PLLA_STARTUP_TIME				0b111111


#define CLOCK_USB_PLL_STARTUP_TIME			0b1111


#define CLOCK_USB_FULL_SPEED_DIVIDER		5


//--------------------------------------------------------------------------------------------------//


// SysTick timer configuration

// This value should be set to the CPU frequency
#define SYSTICK_FREQUENCY					CPU_FREQUENCY


//--------------------------------------------------------------------------------------------------//


// Check configuration

// Must be 1 to enable the check attribute
#define CHECK_ENABLE						1

// Controls whether check triggers a breakpoint
#define CHECK_ENABLE_BREAKPOINT				1

// Controls whether check prints line number and file name
#define CHECK_ENABLE_PRINT_HANDLER			1

// The print handler must have the format print_function(char*, ...)
#define CHECK_PRINT_HANDLER					board_serial_programming_print


//--------------------------------------------------------------------------------------------------//


// Kernel configuration

// Defines the maximum allowed thread name length
#define KERNEL_THREAD_MAX_NAME_LENGTH		32

// xPSR register of the threads
#define KERNEL_THREAD_xPSR					0x01000000

// Link register of the threads
#define KERNEL_THREAD_LR					0x00000000

// Specifies the stack size allocated for the idle thread
#define KERNEL_IDLE_THREAD_STACK_SIZE		100

// Frequency of the kernel interrupt
#define KERNEL_TICK_FREQUENCY				1000

// Statistics will be calculated every 1000 context switched
#define KERNEL_STATISTICS_FREQUENCY			1000


#define KERNEL_FAULT_ENABLE_PRINT_HANDLER	1


#define KERNEL_FAULT_PRINT_HANDLER			board_serial_print


#define DMA_INTERRUPT_PRIORITY				IRQ_LEVEL_3


//--------------------------------------------------------------------------------------------------//


// Serial print configuration

#define SERIAL_PRINTF_BUFFER_SIZE 512


#define SERIAL_DMA_BUFFER_SIZE 1024


//--------------------------------------------------------------------------------------------------//

#endif
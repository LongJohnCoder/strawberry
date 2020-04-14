// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "kernel.h"
#include "watchdog.h"
#include "interrupt.h"
#include "config.h"
#include "clock.h"
#include "flash.h"
#include "board_serial.h"
#include "board_button.h"
#include "board_serial_programming.h"
#include "systick.h"
#include "dynamic_memory.h"
#include "kernel.h"
#include "board_sd_card.h"
#include "command_line_interface.h"
#include "file_system_fat.h"
#include "dram.h"
#include "dma.h"
#include "thread.h"
#include "gpio.h"


//--------------------------------------------------------------------------------------------------//


// Cache dependencies
#include <core_cm7.h>


//--------------------------------------------------------------------------------------------------//


void kernel_startup(void)
{
	// Disable interrupt under system configuration
	interrupt_global_disable();
	
	
	// Disable SysTick
	// The kernel will pend the PendSV handler and perform a context
	// switch if the SysTick exception is executed. If this happens 
	// before the system and kernel is configured, the system will
	// crash. Therefor we must disable interrupt and SysTick until
	// right before we launch the scheduler.
	systick_disable();
	
	
	// Disable watchdog
	watchdog_disable();
	
	
	// Set flash wait states before boosting the clock
	flash_set_wait_states(FLASH_NUMBER_OF_WAIT_STATES);
	
	
	// Configure the clock network
	clock_sources_config(CLOCK_SOURCE_CRYSTAL, CLOCK_CRYSTAL_STARTUP_TIME);
	clock_main_clock_config(CLOCK_SOURCE_CRYSTAL);
	clock_pll_config(CLOCK_PLLA_MULTIPLIER, CLOCK_PLLA_DIVISION, CLOCK_PLLA_STARTUP_TIME);
	clock_master_clock_config(CLOCK_MASTER_CLOCK_SOURCE_PLLA_CLOCK, CLOCK_MASTER_CLOCK_PRESCALER_1, CLOCK_MASTER_CLOCK_DIVISION_DIV_2);


	// Enable I-cache and D-cache
	SCB_EnableICache();
	SCB_EnableDCache();
	SCB_CleanInvalidateDCache();
	
	
	// Start the dynamic memory
	// WARNING: Hard fault will occur if the dynamic memory is not configured
	dram_config();
	dynamic_memory_config();
	
	
	// Configure the DMA core
	dma_config();
	
	
	// Start the serial interfaces
	// Due to developing the system we use several serial interfaces
	// 
	// The serial is used for file system and general debugging. Does not support visual studio interface
	//
	// The serial programming are used for programming and runtime stats printing.
	// It is multiplexed with the display lines and must be deactivated is display is used.
	//
	// The UART is used for mouse / keyboard interface until the USB stack is done
	board_serial_config();
	board_serial_programming_config();
	
	
	// Configure the on board button
	board_button_config();
	
	
	// Start the graphics core	
	//graphics_driver_config();

	
	// Now we start up the kernel and its components
	thread_config();
	
	
	// Starts the whole files system in its own thread
	file_system_command_line_config();
	
	
	// Configure the led
	gpio_set_pin_function(PIOC, 8, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOC, 8);
}


//--------------------------------------------------------------------------------------------------//
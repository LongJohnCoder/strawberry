// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "dynamic_loader.h"
#include "scheduler.h"
#include "config.h"
#include "board_serial.h"
#include "dynamic_memory.h"
#include "usart.h"
#include "thread.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	FAST_PROGRAMMING_IDLE,
	FAST_PROGRAMMING_SIZE,
	FAST_PROGRAMMING_SIZE_PLUS,
	FAST_PROGRAMMING_DATA,
	FAST_PROGRAMMING_CHECK
} kernel_fast_programming_state;


//--------------------------------------------------------------------------------------------------//



volatile kernel_fast_programming_state fast_programming_state = FAST_PROGRAMMING_IDLE;
uint8_t program_size_index;
uint32_t program_size;
uint32_t program_index;
uint32_t program_size_total;
uint8_t* program_buffer;


//--------------------------------------------------------------------------------------------------//


void delete_handler(void)
{
	dynamic_memory_free(program_buffer);
}


//--------------------------------------------------------------------------------------------------//


void dynamic_loader_run(uint32_t* data, uint32_t size)
{
	// Start with relocating the .GOT and .GOT PLT table addresses
	dynamic_loader_relocate(data);
	
	// This is the address that contains the entry for the program
	uint8_t* program_entry = (uint8_t *)((uint8_t *)data + *(data + 1));
	
	// The processor supports only thumb mode so we have to make sure bit 1 in the program counter
	// is set to one. Otherwise we will get a hard fault.
	program_entry = (uint8_t *)((uint32_t)program_entry | 1);
	
	// Add offset
	data += 6;
	
	// Get the stack size, name length and name pointer
	uint32_t stack_size = (uint32_t)(*data++);
	uint32_t name_length = (uint32_t)(*data++);
	char* name = (char *)data;
	
	// Check if the name if valid and start the thread
	if (dynamic_loader_check_name(name, name_length))
	{
		scheduler_set_dynamic_loader_handler(delete_handler);
		struct thread_structure* tmp = thread_new(name, (thread_function)program_entry, NULL, THREAD_PRIORITY_NORMAL, stack_size);
		tmp->ID = 6969;
	}

}


//--------------------------------------------------------------------------------------------------//


uint8_t dynamic_loader_check_name(char* data, uint32_t size)
{
	if (size > KERNEL_THREAD_MAX_NAME_LENGTH)
	{
		
		return 0;
	}
	
	for (uint32_t i = 0; i < size; i++)
	{
		if (*data++ == '\0')
		{
			return 1;
		}
	}
	
	return 0;
}


//--------------------------------------------------------------------------------------------------//


void dynamic_loader_relocate(uint32_t* data)
{
	// This functions will resolve the absolute addressing in the application.
	// It will relocate the entries in the .GOT and .GOT PLT section
	
	uint8_t* program_start = (uint8_t *)data;
	
	// Skip the application entry
	data++;
	data++;
	
	// Get the offsets to the .GOT table
	uint8_t* start_got = program_start + *data++;
	uint8_t* end_got = program_start + *data++;
	
	// Get the offsets to the .GOT PLT table
	uint8_t* start_got_plt = program_start + *data++;
	uint8_t* end_got_plt = program_start + *data++;
	
	
	// Resolve .GOT addressed by performing dynamic linking
	for (uint32_t* relocate_interator = (uint32_t *)start_got; relocate_interator < (uint32_t *)end_got; relocate_interator++)
	{
		// Add the global offset to each entry
		*relocate_interator += (uint32_t)program_start;
	}
	
	// Resolve .GOT PLT addressed by performing dynamic linking
	for (uint32_t* relocate_interator = (uint32_t *)start_got_plt; relocate_interator < (uint32_t *)end_got_plt; relocate_interator++)
	{
		// Add the global offset to each entry
		*relocate_interator += (uint32_t)program_start;
	}
}


//--------------------------------------------------------------------------------------------------//

// This USART handler is used for the fast programming interface.
// This interface will dynamically download a user program and run it immediately.
// Deleting of the program is not handled. This can be done by a hardware reset.

void USART0_Handler()
{
	// The RXRDY flag is cleared upon read of RHR
	char data = (char)usart_read(USART0);
	
	if (fast_programming_state == FAST_PROGRAMMING_IDLE)
	{
		if (data == 'P')
		{
			// Start byte received
			board_serial_print("Programming started\n");
			fast_programming_state = FAST_PROGRAMMING_SIZE;
			program_size_index = 0;
			program_size = 0;
			program_index = 0;
			program_size_total = 0;
		}
	}
	else if (fast_programming_state == FAST_PROGRAMMING_SIZE)
	{
		program_size |= (data << (8 * program_size_index++));
		
		if (program_size_index >= 4)
		{
			fast_programming_state = FAST_PROGRAMMING_SIZE_PLUS;
			program_size_index = 0;
		}
	}
	else if (fast_programming_state == FAST_PROGRAMMING_SIZE_PLUS)
	{
		program_size_total |= (data << (8 * program_size_index++));
		
		if (program_size_index >= 4)
		{
			program_buffer = (uint8_t *)dynamic_memory_new(DRAM_BANK_0, program_size_total + 255);
			
			fast_programming_state = FAST_PROGRAMMING_DATA;
		}
	}
	else if (fast_programming_state == FAST_PROGRAMMING_DATA)
	{		
		program_buffer[program_index] = data;
		
		program_index++;
		if (program_index >= program_size)
		{
			fast_programming_state = FAST_PROGRAMMING_CHECK;
		}
	}
	else if (fast_programming_state == FAST_PROGRAMMING_CHECK)
	{
		if (data == 'P')
		{
			board_serial_print("Programming success\n");
			
			SCB_CleanDCache();
			
			dynamic_loader_run((uint32_t *)program_buffer, program_size);
		}
		
		fast_programming_state = FAST_PROGRAMMING_IDLE;
	}
}


//--------------------------------------------------------------------------------------------------//
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "syscall.h"
#include "board_serial.h"
#include "kernel.h"
#include "gpio.h"


//--------------------------------------------------------------------------------------------------//


// Attribute for the SVC handler
#define SYSCALL_NAKED __attribute__((naked))


// Macro for calling the SVC handler with the specified SVC number
#define SYSCALL(svc_number) asm volatile ("svc %[arg]" : : [arg] "I" (svc_number))


//--------------------------------------------------------------------------------------------------//


void SYSCALL_NOINLINE syscall_print(char* data)
{
	SYSCALL(SYSCALL_PRINT);
}


//--------------------------------------------------------------------------------------------------//


void SYSCALL_NOINLINE syscall_sleep(uint32_t ticks)
{
	SYSCALL(SYSCALL_DELAY);
}


//--------------------------------------------------------------------------------------------------//


void SYSCALL_NOINLINE syscall_gpio_status(Pio* port, uint32_t* reg)
{
	SYSCALL(SYSCALL_GPIO_STATUS);
}


//--------------------------------------------------------------------------------------------------//


// The kernel services runs in from an interrupt handler with low priority
// (lowest of the cortex-m7 implemented interrupts) and should therefore execute
// as fast as possible. If a service takes long to execute, it will destroy
// the system, and the kernel functionality

void kernel_service_handler(uint32_t* svc_argv)
{
	// Extract the SVC number from the parameter list
	uint32_t svc_number = ((char *)svc_argv[6])[-2];

	switch (svc_number)
	{
		case SYSCALL_DELAY:
			thread_delay((uint32_t)svc_argv[0]);
			break;
		
		case SYSCALL_PRINT:
			board_serial_print((char*)svc_argv[0]);
			break;
		case SYSCALL_GPIO_STATUS:
			*((uint32_t *)svc_argv[1]) = gpio_get_pin_value_status_register((Pio *)svc_argv[0]);
			break;
	}
}


//--------------------------------------------------------------------------------------------------//
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "fault.h"
#include "config.h"
#include "board_serial.h"


//--------------------------------------------------------------------------------------------------//


void MemoryManagement_Handler()
{
	uint32_t memory_fault_register = *(uint8_t *)0xE000ED28;

	board_serial_print("Memory manage fault handler triggered\n\n");

	board_serial_print("MMARVALID\t%d\n", (memory_fault_register & (1 << 7)) ? 1 : 0);
	board_serial_print("MLSPERR\t%d\n", (memory_fault_register & (1 << 5)) ? 1 : 0);
	board_serial_print("MSTKERR\t%d\n", (memory_fault_register & (1 << 4)) ? 1 : 0);
	board_serial_print("MUNSTKERR\t%d\n", (memory_fault_register & (1 << 3)) ? 1 : 0);
	board_serial_print("DACCVIOL\t%d\n", (memory_fault_register & (1 << 1)) ? 1 : 0);
	board_serial_print("IACCVIOL\t%d\n", (memory_fault_register & (1 << 0)) ? 1 : 0);
}


//--------------------------------------------------------------------------------------------------//


void BusFault_Handler()
{
	uint32_t bus_fault_register = *(uint8_t *)0xE002ED29;

	board_serial_print("Bus fault handler triggered\n\n");

	board_serial_print("BFARVALID\t%d\n", (bus_fault_register & (1 << 7)) ? 1 : 0);
	board_serial_print("LSPERR\t%d\n", (bus_fault_register & (1 << 5)) ? 1 : 0);
	board_serial_print("STKERR\t%d\n", (bus_fault_register & (1 << 4)) ? 1 : 0);
	board_serial_print("UNSTKERR\t%d\n", (bus_fault_register & (1 << 3)) ? 1 : 0);
	board_serial_print("IMPRECISERR\t%d\n", (bus_fault_register & (1 << 2)) ? 1 : 0);
	board_serial_print("PRECISERR\t%d\n", (bus_fault_register & (1 << 1)) ? 1 : 0);
	board_serial_print("IBUSERR\t%d\n", (bus_fault_register & (1 << 0)) ? 1 : 0);
}


//--------------------------------------------------------------------------------------------------//


void UsageFault_Handler()
{
	uint32_t usage_fault_register = *(uint16_t *)0xE000ED2A;

	board_serial_print("Usage fault handler triggered\n\n");

	board_serial_print("DIVBYZERO\t\t%d\n", (usage_fault_register & (1 << 9)) ? 1 : 0);
	board_serial_print("UNALIGNED\t\t%d\n", (usage_fault_register & (1 << 8)) ? 1 : 0);
	board_serial_print("NOCP\t\t\t%d\n", (usage_fault_register & (1 << 3)) ? 1 : 0);
	board_serial_print("INVPC\t\t\t%d\n", (usage_fault_register & (1 << 2)) ? 1 : 0);
	board_serial_print("INVSTATE\t\t%d\n", (usage_fault_register & (1 << 1)) ? 1 : 0);
	board_serial_print("UNDEFINESTR\t%d\n", (usage_fault_register & (1 << 0)) ? 1 : 0);

}


//--------------------------------------------------------------------------------------------------//


void HardFault_Handler()
{
	uint32_t usage_fault_register = *(uint16_t *)0xE000ED2A;
	uint32_t bus_fault_register = *(uint8_t *)0xE002ED29;
	uint32_t memory_fault_register = *(uint8_t *)0xE000ED28;
	
	board_serial_print_register("\n\nBus: ", bus_fault_register);
	board_serial_print_register("Memory: ", memory_fault_register);
	board_serial_print_register("Usage: ", usage_fault_register);
	
	while (1);
}


//--------------------------------------------------------------------------------------------------//
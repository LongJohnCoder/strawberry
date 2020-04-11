// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef DYNAMIC_MEMORY_H
#define DYNAMIC_MEMORY_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "config.h"


//--------------------------------------------------------------------------------------------------//


// This memory implementation uses the 4 most significant bits of the memory block size to determine
// the memory section and memory status. This can be quite confusing when debugging the system.
// The table below shows how to interpret the info-byte.
//
// Due to big-endian data storage, the memory size will be shown like below in debug sessions,
// where the last byte represents the info we want
//                         __
// 0x00000000  00  00  00  00
//
//------------------------------------------------------//
// Memory section	|	Memory used		|	Memory free	//
//------------------------------------------------------//
// SRAM				|		80			|		00		//
//------------------------------------------------------//
// DRAM Bank 0		|		90			|		10		//
//------------------------------------------------------//
// DRAM Bank 1		|		A0			|		20		//
//------------------------------------------------------//
// Section 3		|		B0			|		30		//
//------------------------------------------------------//
// Section 4		|		C0			|		40		//
//------------------------------------------------------//
// Section 5		|		D0			|		50		//
//------------------------------------------------------//
// Section 6		|		E0			|		60		//
//------------------------------------------------------//
// Section 7		|		F0			|		70		//
//------------------------------------------------------//


//--------------------------------------------------------------------------------------------------//


// List of sections with dynamic memory support
// This will be the input to all dynamic memory functions

typedef enum
{
	SRAM,
	DRAM_BANK_0,
	DRAM_BANK_1
} Dynamic_memory_section;


//--------------------------------------------------------------------------------------------------//


// This dynamic memory implementation uses a lightweight algorithm and is optimized for
// allocations across multiple sections. The user can request dynamic memory and specify where to
// put it, and the memory driver will automatically allocate memory in that section. To delete
// memory only the pointer is required.
//
// This implementation uses only free blocks to navigate in memory, thus increasing allocation
// speed. It combines adjacent memory block to prevent memory fragmentation.

// This is the dynamic memory descriptor
// Every block that is allocated or freed will start with this descriptor

typedef struct dynamic_memory_descriptor_s
{
	// Pointer to next element in the linked list
	struct dynamic_memory_descriptor_s* next;
	
	// Size of the memory block
	uint32_t size;
		
} dynamic_memory_descriptor;


//--------------------------------------------------------------------------------------------------//


// Dynamic memory section descriptor
typedef struct
{
	// Name of the section
	char name[DYNAMIC_MEMORY_SECTION_NAME_SIZE];
	
	// The section addresses does not need to be aligned
	// since this is handled by software
	uint32_t start_address;
	uint32_t end_address;
	
	// Specifies the total and free size
	uint32_t total_memory;
	uint32_t free_memory;
	
	// Configuration for the dynamic memory section
	uint32_t minimum_block_size;
	uint32_t allignment;
	
	// Define descriptors
	dynamic_memory_descriptor start_descriptor_object;
	dynamic_memory_descriptor* start_descriptor;
	dynamic_memory_descriptor* end_descriptor;
	
} Dynamic_memory_section_s;


//--------------------------------------------------------------------------------------------------//


void dynamic_memory_config(void);

void* dynamic_memory_new(Dynamic_memory_section memory_section, uint32_t size);

void dynamic_memory_free(void* memory_object);


//--------------------------------------------------------------------------------------------------//


uint32_t dynamic_memory_get_total_size(Dynamic_memory_section memory_section);

uint32_t dynamic_memory_get_used_size(Dynamic_memory_section memory_section);

uint32_t dynamic_memory_get_free_size(Dynamic_memory_section memory_section);

uint8_t dynamic_memory_get_used_percentage(Dynamic_memory_section memory_section);


//--------------------------------------------------------------------------------------------------//


#endif
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "dynamic_memory.h"
#include "check.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>
#include <string.h>
#include <core_cm7.h>


//--------------------------------------------------------------------------------------------------//


// Variables declared extern in the linker script
// These mark the beginning and end of the linkers heap section
extern uint32_t _sheap;
extern uint32_t _eheap;


//--------------------------------------------------------------------------------------------------//


// Below is the memory section definitions. The first section is located in
// internal SRAM and gets the addresses from the linker script. Thus a start
// and end address of zero is added.
//
// The alignment can be MAX 8 bytes - this will not be fixed
 
Dynamic_memory_section_s dynamic_section_sram =
{
	.start_address		= 0,
	.end_address		= 0,
	.allignment			= 8,
	.minimum_block_size = 8,
	.name				= "SRAM"
};


Dynamic_memory_section_s dynamic_section_dram_bank_0 = 
{
	.start_address		= 0x70000000,
	.end_address		= 0x7007FFFF,
	.allignment			= 8,
	.minimum_block_size = 8,
	.name				= "DRAM bank 0"
};


Dynamic_memory_section_s dynamic_section_dram_bank_1 =
{
	.start_address		= 0x70080000,
	.end_address		= 0x700FFFFF,
	.allignment			= 8,
	.minimum_block_size = 8,
	.name				= "DRAM bank 1"
};


Dynamic_memory_section_s* dynamic_memory_sections[] = 
{
	&dynamic_section_sram,
	&dynamic_section_dram_bank_0,
	&dynamic_section_dram_bank_1,
	NULL
};


//--------------------------------------------------------------------------------------------------//


// Memory section operations
#define MEMORY_IS_BLOCK_USED(size)				((size) & 0x80000000)
#define MEMORY_SET_BLOCK_USED(size)				((size) | 0x80000000)
#define MEMORY_SET_BLOCK_FREE(size)				((size) & 0x7FFFFFFF)
#define MEMORY_GET_RAW_SIZE(size)				((size) & 0xfffffff)


// This bit mask defines how many memory sections that is allowed
#define MEMORY_GET_SECTION(size)				(((size) >> 28) & 0b111)
#define MEMORY_SET_SECTION(size, sect)			(((sect) << 28) | (size))


//--------------------------------------------------------------------------------------------------//


// Specifies the size of the dynamic memory descriptor
const uint8_t memory_descriptor_size = ((sizeof(dynamic_memory_descriptor) + 8 - 1) & ~(8 - 1));


//--------------------------------------------------------------------------------------------------//


// Private prototypes
static void dynamic_memory_insert_block(Dynamic_memory_section memory_section, dynamic_memory_descriptor* block);


//--------------------------------------------------------------------------------------------------//


void dynamic_memory_config(void)
{
	uint32_t section_counter = 0;

	Dynamic_memory_section_s* it = dynamic_memory_sections[section_counter];
	
	it->start_address = (uint32_t)(&_sheap);
	it->end_address = (uint32_t)(&_eheap);
	
	while (it != NULL)
	{		
		// Zero-initializes the memory sections
		// Due to a bug the Cortex-M7 only responds to 8-bit and 32-bit accesses here
		volatile uint8_t* start = (volatile uint8_t *)it->start_address;
		volatile uint8_t* stop = (volatile uint8_t *)it->end_address;

		while (start != stop)
		{
			*start++ = 0x00;
		}
		
		// We must align the start and end address		
		if (it->start_address & (it->allignment - 1))
		{
			it->start_address = ((it->start_address + it->allignment - 1) & ~(it->allignment - 1));
		}
		
		if (it->end_address & (it->allignment - 1))
		{
			it->end_address &= ~(it->allignment - 1);
		}
		
		// Make space for the end descriptor
		it->end_address -= memory_descriptor_size;
		
		// Calculate the total section memory
		// The total memory should be the total first block size
		it->total_memory = it->end_address - it->start_address;
		
		// The total free memory is the total block size minus the descriptor
		it->free_memory = it->total_memory;

		// Configure the memory section descriptors
		it->start_descriptor = &it->start_descriptor_object;
		it->end_descriptor = (dynamic_memory_descriptor *)it->end_address;
		it->start_descriptor->next = (dynamic_memory_descriptor *)it->start_address;
		
		it->start_descriptor->next->next = it->end_descriptor;
		it->start_descriptor->next->size = it->total_memory;
		it->start_descriptor->next->size = MEMORY_SET_BLOCK_FREE(it->start_descriptor->next->size);
		
		it->end_descriptor->next = NULL;
		it->end_descriptor->size = 0;
		it->end_descriptor->size = MEMORY_SET_BLOCK_USED(it->end_descriptor->size);
		
		it = dynamic_memory_sections[++section_counter];
	}
}


//--------------------------------------------------------------------------------------------------//


static void dynamic_memory_insert_block(Dynamic_memory_section memory_section, dynamic_memory_descriptor* block)
{
	// Retrieve the right memory section
	Dynamic_memory_section_s* current_section = dynamic_memory_sections[memory_section];
	
	// Check that the block is passed right
	check((block < current_section->end_descriptor) && (block >= current_section->start_descriptor));
	
	// Make a iterator
	dynamic_memory_descriptor* block_iterator;
	
	for (block_iterator = current_section->start_descriptor; block_iterator->next < block; block_iterator = block_iterator->next)
	{
		// Do nothing
	}
	
	// Now the block iterator points to the memory block before the insertion
	// block_iterator->next points to the memory block after the insertion
	// Create a pointer to the inserted block address
	uint8_t* addr = (uint8_t *) block_iterator;
	
	uint32_t acctual_block_size = MEMORY_GET_RAW_SIZE(block->size);
	uint32_t acctual_block_iterator_size = MEMORY_GET_RAW_SIZE(block_iterator->size);
	
	// The algorithm rely on the fact that every element in the list contains free memory
	// We therefore have to check whether the previous free block overlaps with the new block
	// In this case the block will be combined 
	if ((addr + acctual_block_iterator_size) == ((uint8_t *)block))
	{
		block_iterator->size += acctual_block_size;
		block = block_iterator;
	}
	
	// Now the current block is merged with the previous block. The current block has then the address of the previous block
	// Check if the previous (maybe merged) block overlaps with the next memory block
	
	addr = (uint8_t *) block;
	
	acctual_block_size = MEMORY_GET_RAW_SIZE(block->size);
	
	if ((addr + acctual_block_size) == ((uint8_t *)block_iterator->next))
	{
		// Check if the next block is the last block
		// In this case do not merge blocks
		if (block_iterator->next != current_section->end_descriptor)
		{
			block->size += MEMORY_GET_RAW_SIZE(block_iterator->next->size);
			block->next = block_iterator->next->next;
		}
		else
		{
			block->next = current_section->end_descriptor;
		}
	}
	else
	{
		block->next = block_iterator->next;
	}
	
	// Last, if the inserted memory was not merged with the previous block
	// make the previous block point to the inserted block
	if (block != block_iterator)
	{
		block_iterator->next = block;
	}
}


//--------------------------------------------------------------------------------------------------//


void* dynamic_memory_new(Dynamic_memory_section memory_section, uint32_t size)
{
	// Pointer to iterators
	dynamic_memory_descriptor* block_iterator_previous;
	dynamic_memory_descriptor* block_iterator_current;
	
	// Pointer to the memory section
	Dynamic_memory_section_s* current_section = dynamic_memory_sections[memory_section];
	
	// void pointer that the function will return
	void* return_value = NULL;
	
	// Check that the size is greater than zero
	check(size != 0);
	
	// Make sure the size requested is greater than the minimum value
	if (size < current_section->minimum_block_size)
	{
		size = current_section->minimum_block_size;
	}
	
	// Allocate new memory
	// First add the size of the descriptor and add alignment padding
	size += memory_descriptor_size;
	
	
	// Make sure the size has the required alignment
	if (size & (current_section->allignment - 1))
	{
		size = ((size + current_section->allignment - 1) & ~(current_section->allignment - 1));
	}
	
	// Now the correct size of the block to be allocated is determined. This size
	// includes the memory descriptor in the start
	
	// If there is enough bytes remaining in the heap section
	// The free memory size holds the number of free bytes. That is not include
	if (size < current_section->free_memory)
	{
		// Iterate through list and try to find a match
		block_iterator_previous = current_section->start_descriptor;
		block_iterator_current = current_section->start_descriptor->next;
		
		while ((MEMORY_GET_RAW_SIZE(block_iterator_current->size) < size) && (block_iterator_current->next != NULL))
		{
			block_iterator_previous = block_iterator_current;
			block_iterator_current = block_iterator_current->next;
		}
				
		
		// After the while loop the block_iterator_current points to the memory block
		// that is large enough to hold the requested size.
		
		// Check that the block is not the end block 
		if (block_iterator_current != current_section->end_descriptor)
		{
			// The return value should have an offset big enough to hold the memory descriptor
			return_value = ((void *)(((uint8_t *)block_iterator_current) + memory_descriptor_size));
			
			
			// Take out the available block of the linked list
			block_iterator_previous->next = block_iterator_current->next;
			
			if ((MEMORY_GET_RAW_SIZE(block_iterator_current->size) - size) >= (current_section->minimum_block_size + memory_descriptor_size))
			{
				// The block has bigger size than the requested size 
				// AND the remained has also a bigger size than the minimum size
				// This is to prevent very tiny blocks of available unused memory
				// often called memory fragmentation
				dynamic_memory_descriptor* block_to_insert = ((dynamic_memory_descriptor *)(((uint8_t *)block_iterator_current) + size));
				
				check((((uint32_t)block_to_insert) & (current_section->allignment - 1)) == 0);
				
				// Calculate the size of the two blocks
				block_to_insert->size = block_iterator_current->size - size;
				block_iterator_current->size = size;
				
				// Insert the free block into the list of free elements
				dynamic_memory_insert_block(memory_section, block_to_insert);
			}
			
			// Updated the free bytes remaining
			current_section->free_memory -= size;

			// The block is still there but we have to tell the kernel that
			// it is not used anymore
			block_iterator_current->next = NULL;
			block_iterator_current->size = MEMORY_SET_BLOCK_USED(block_iterator_current->size);
			
			// In order to free the memory just by the pointer we need to
			// add some information in order to know what memory section
			// it belongs to
			block_iterator_current->size = MEMORY_SET_SECTION(block_iterator_current->size, memory_section);
		}
	}
	
	if (return_value == NULL)
	{
		// Allocation failed
		// This means that the memory is full or a bug has been discovered
		// Call the fault handler to try to fix the bug
		//check(0); // REMOVE
	}
	
	uint8_t* tmp = (uint8_t *)return_value;
	
	// Fill memory with zeros
	for (uint32_t i = 0; i < (size - 8); i++)
	{
		*tmp++ = 0;
	}
	
	return return_value;
}


//--------------------------------------------------------------------------------------------------//


void dynamic_memory_free(void* memory_object)
{
	dynamic_memory_descriptor* block;
	
	// Check if the object pointed to is not zero
	if (memory_object != NULL)
	{
		// Every object has a memory descriptor right behind it
		// on an active memory block the next pointer should be zero
		memory_object = (void *)((uint8_t *)memory_object - memory_descriptor_size);
		
		// Cast the address to a memory object
		block = (dynamic_memory_descriptor *)memory_object;
		
		if (block->next == NULL)
		{
			if (MEMORY_IS_BLOCK_USED(block->size))
			{
				// Check which memory section it belongs to
				Dynamic_memory_section sect = MEMORY_GET_SECTION(block->size);
				
				// Remove the memory free bit
				block->size = MEMORY_SET_BLOCK_FREE(block->size);
				
				// Now updated the number of free bytes in the heap
				dynamic_memory_sections[sect]->free_memory += MEMORY_GET_RAW_SIZE(block->size);
				
				// Insert the block in the linked list of free elements
				dynamic_memory_insert_block(sect, block);
				
				SCB_CleanDCache();
			}
			
			// If any of the below checks are hit by the processor the memory is lost
			// The user might have lost the memory pointer or changed the two 32-bit fields
			// right behind the memory. 
			//
			// TODO: Memory callback, maybe reset, maybe terminate program
			
			else
			{
				check(0);
			}
		}
		else
		{
			check(0);
		}
	}
	else
	{
		check(0);
	}
}


//--------------------------------------------------------------------------------------------------//


uint32_t dynamic_memory_get_total_size(Dynamic_memory_section memory_section)
{
	uint32_t tmp = dynamic_memory_sections[memory_section]->total_memory;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t dynamic_memory_get_used_size(Dynamic_memory_section memory_section)
{
	uint32_t tmp_total = dynamic_memory_sections[memory_section]->total_memory;
	uint32_t tmp_free = dynamic_memory_sections[memory_section]->free_memory;
	
	return (tmp_total - tmp_free);
}


//--------------------------------------------------------------------------------------------------//


uint32_t dynamic_memory_get_free_size(Dynamic_memory_section memory_section)
{
	uint32_t tmp_free = dynamic_memory_sections[memory_section]->free_memory;
	
	return tmp_free;
}


//--------------------------------------------------------------------------------------------------//


uint8_t dynamic_memory_get_used_percentage(Dynamic_memory_section memory_section)
{
	// Get the memory info
	uint32_t tmp_total = dynamic_memory_sections[memory_section]->total_memory;
	uint32_t tmp_free = dynamic_memory_sections[memory_section]->free_memory;
	
	// Calculate the percentage
	uint32_t tmp_percent = ((tmp_total - tmp_free) * 100) / tmp_total;
	
	return (uint8_t)tmp_percent;
}


//--------------------------------------------------------------------------------------------------//
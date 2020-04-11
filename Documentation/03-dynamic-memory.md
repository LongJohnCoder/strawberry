# Dynamic Memory

Dynamic memory is used when an execution program asks the operating system to give it a block of the main memory. This allocation happends at runtime, which means the the kernel does not know about the allocation until it happends. There are many different implementations of dynamic memory, each has some advantages and some disadvantages. The allocation algorithm used in the BlackOS is based upon explicit free lists algorithm. 

## Introduction

The dynamic memory algorithm is based upon continous tracking of free memory blocks. A memory block is a pysical unit of memory, and all the memory blocks forms a linked list. The list is sorted by address, whith the lowest address first. Allocated memory is removed from the list, and is not tracked by the kernel, while memory that is freed gets insterted into the list and gets tracked by the kernel. After reset, the memory consists of one big free block of memory. When the user asks for memory, the kernel starts iterating through the list of free blocks. The kernel stops when it finds a free block that is large enough to hold the requested memory. If the requested memory is less than the free block, the kernel will try to split the free block. The bottom part is removed from the list (allocated), and the top part becomes a new free block. This only happends if the remaining top block is large enough to contain a nwe allocation. 

When the memory is beeing freed, the kernel just inserts is into the list of free blocks. The kernel will check the previous block and the next block, and check if any of them overlaps with the inserted block. In this is the case, they will be combined into a single large block.

# Memory layout

The processor can have several memory sources. In our case we have the internal SRAM and four external DRAM banks. Due to refresh penalties in the dynamic memory, code and data should be placed in different banks. This is of course somthing that the dynamic memory must take into acccount. It must be able to allocate data in a specified memory section. 

The dynamic memory starts by specifying some memory section. A memory section is configured by doing three things. First the start address, stop address, allignment and minimum block size is put into a struct. Then this struct are placed in an array (which the kernel use). Lastly the memory section must be referenced in an enum. This enum is passed as an argument to the allocation function. 

This is the struct that specifies the parameters for the memory section
```c
Dynamic_memory_section_s dynamic_section_dram_bank_0 = 
{
	.start_address		= 0x70000000,
	.end_address		= 0x7007FFFF,
	.allignment		= 8,
	.minimum_block_size     = 8,
	.name			= "DRAM bank 0"
};
```

The struct is then added to this list so that the kernel can reference it
```c
Dynamic_memory_section_s* dynamic_memory_sections[] = 
{
	&dynamic_section_sram,
	&dynamic_section_dram_bank_0,
	&dynamic_section_dram_bank_1,
	NULL
};
```

This is used by the calling application to ensure that the data is allocated in the right section
```c
typedef enum
{
	SRAM,
	DRAM_BANK_0,
	DRAM_BANK_1
} Dynamic_memory_section;
```

## Memory descriptor

Before every memory block is a memory descriptor. The descriptor consists of 8 bytes, and contains information about the memory block. The first four bytes points to another memory descriptor. The last four bytes contains the size. Since the block size never will exceed a 32-bits number, we can use the upper bits to store aditional information. Bit 31 determines if the memory block is used or free. Bits [30:28] stores which memory section that is used. Since the memory free function only takes in a pointer to the memory, we have to determine from which memory section we should free the memory. Bit [30:28] tells us that. If a memory block is free, the memory descriptor will point to the next free block. If the memory block is used the memory descriptor will contain the NULL pointer. The memory descriptors of *free* blocks form linked list which will be used by the code. An illistration can be found below

<img src="https://github.com/bjornbrodtkorb/BlackOS/blob/master/BlackOS%20Graphics/list.png" width="1000">

## Configuration

After reset the kernel initializes all memory sections. After the configuration all the sections should consist of one large free memory block. The start address and the end address is first aligned with the preffered alignment. The whole memory section is then written to zero. The memory section descriptor should provide information about the memory. This includes a pointer to the first element in the linked list of free blocks.

## Allocation

Allocation of memory will conceptually remove blocks (or part of block) from the memory list. The list will be sorted in the same way the memory is sorted. That means that a block at address 0x90 will never point to a list item at address 0xFF. This way it is easy to iterate the list and find the first match. This list only contains free memory blocks. If memory is requested, the code iterated trough blocks in the list until it finds a block that is large enough to hold the memory. If the sizes are equal - perfect, but if the list block size are bigger than the requested memory, the remaining part has to be put back into the list. The block is then split, and the part not used it put back into the free list. This mechanism requires that the remaining block is large enough to hold both a memory descriptor + the minimum block size. 

<img src="https://github.com/bjornbrodtkorb/BlackOS/blob/master/BlackOS%20Graphics/memory_layout.png" width="1000">

## Free

Freeing of memory happend in somewhat the same manner as allocation, but instead of removing items from the list, we are adding. An inserted memory block is first validated. That includes

- check if the next block pointer is NULL
- check that the free-bit is cleared
- retrieve the memory section information

After the memory block is validated the code iterates through the list. When it finds a element with a lower address, it inserts the block after this element. When this block is inserted we have two scenarios

- the previous block overlaps with the inserted block
- the inserted block overlaps with the next block

Let me rephrase overlaps. A memory block overlaps with another if the remaining "space" between them doesn't fit both a memory descriptor + a minimum block size. If blocks overlaps, the code will manipulate pointers and merge the block together, into a bigger block, thus reducing memory fregmentation. 

## Memory leaks

Memory leaks happens when the dynamic memory interface allocates data, and the pointer to that data is lost. This will result in unusable sections inside the memory. Some problems occur when the operating system want to terminate an ongoing process. If this process has allocated memory, and the thread is terminated by force, the memory is lost. Idelly the operating system will request the appliction to close. The application may have a exit callback thats deletes all memory allocated. In this way the memory will be preserved. 

If memory leaks has occured, the only way to restore the memory is by a soft / hard reset of the computer. If the memory is not full this will not be a problem.

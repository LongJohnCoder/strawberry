// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "cache.h"
#include "core_cm7.h"


//--------------------------------------------------------------------------------------------------//


void cache_clean(void)
{
	SCB_CleanDCache();
}


//--------------------------------------------------------------------------------------------------//


void cache_clean_addresses(uint32_t* addr, uint32_t size)
{
	// Cache clean by address only works on 32-bit aligned data
	// We therefore have to align the start address and the size
	uint32_t* new_address = (uint32_t *)((uint32_t)addr & ~32);
	uint32_t new_size = (size + 32) & ~32;
	
	SCB_CleanDCache_by_Addr(new_address, new_size);
}


//--------------------------------------------------------------------------------------------------//


void cache_invalidate(void)
{
	SCB_InvalidateDCache();
}


//--------------------------------------------------------------------------------------------------//


void cache_invalidate_addresses(uint32_t* addr, uint32_t size)
{
	// Cache clean by address only works on 32-bit aligned data
	// We therefore have to align the start address and the size
	uint32_t* new_address = (uint32_t *)((uint32_t)addr & ~32);
	uint32_t new_size = (size + 32) & ~32;
	
	SCB_InvalidateDCache_by_Addr(new_address, new_size);
}


//--------------------------------------------------------------------------------------------------//
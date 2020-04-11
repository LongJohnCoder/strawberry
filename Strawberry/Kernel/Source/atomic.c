// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "atomic.h"


//--------------------------------------------------------------------------------------------------//


void atomic_increment(uint32_t* memory)
{
	do 
	{
		uint32_t value = __LDREXW(memory);
		
		if (__STREXW(++value, memory) == 0)
		{
			__DMB();
			return;
		}
		
	} while (1);
}


//--------------------------------------------------------------------------------------------------//


void atomic_decrement(uint32_t* memory)
{
	do
	{
		uint32_t value = __LDREXW(memory);
		
		if (__STREXW(--value, memory) == 0)
		{
			__DMB();
			return;
		}
		
	} while (1);
}


//--------------------------------------------------------------------------------------------------//


void atomic_write(uint32_t* memory, uint32_t value)
{
	do
	{		
		if (__STREXW(value, memory) == 0)
		{
			__DMB();
			
			// Success
			return;
		}
		
	} while (1);
}


//--------------------------------------------------------------------------------------------------//


uint32_t atomic_read(uint32_t* memory)
{
	return __LDREXW(memory);
}


//--------------------------------------------------------------------------------------------------//
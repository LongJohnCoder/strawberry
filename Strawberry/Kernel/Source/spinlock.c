// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "spinlock.h"


//--------------------------------------------------------------------------------------------------//


void spinlock_aquire(struct spinlock* spin)
{
	uint32_t status = 0;
	
	do {
		while (__LDREXW(&(spin->lock)) != 0)
		{
			// Spin
		}
		
		status = __STREXW(1, &(spin->lock));
		
	} while (status != 0);
	
	
	// Do not start any other memory access until DMB has finished
	__DMB();
}


//--------------------------------------------------------------------------------------------------//


void spinlock_release(struct spinlock* spin)
{
	__DMB();


	spin->lock = 0;
}


//--------------------------------------------------------------------------------------------------//
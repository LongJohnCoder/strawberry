// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "mutex.h"
#include "scheduler.h"
#include "atomic.h"


//--------------------------------------------------------------------------------------------------//


void mutex_lock(struct mutex* mutex)
{
	uint32_t status = 0;
	
	do {
		while (__LDREXW(&(mutex->lock)) != 0)
		{
			reschedule();
		}
		
		status = __STREXW(1, &(mutex->lock));
		
	} while (status != 0);
	
	// Do not start any other memory access until DMB has finished
	__DMB();
	
	return;
}


//--------------------------------------------------------------------------------------------------//


void mutex_unlock(struct mutex* mutex)
{
	__DMB();


	mutex->lock = 0;
	

	return;
}


//--------------------------------------------------------------------------------------------------//
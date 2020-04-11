// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef SPINLOCK_H
#define SPINLOCK_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


struct spinlock
{
	volatile uint32_t lock;
	
	void* owner;
};


//--------------------------------------------------------------------------------------------------//


void spinlock_aquire(struct spinlock* spinlock);


//--------------------------------------------------------------------------------------------------//


#endif
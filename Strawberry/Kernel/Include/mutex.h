// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef MUTEX_H
#define MUTEX_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


struct mutex
{
	uint32_t lock;
};


//--------------------------------------------------------------------------------------------------//


void mutex_lock(struct mutex* mutex);

void mutex_unlock(struct mutex* mutex);


//--------------------------------------------------------------------------------------------------//


#endif
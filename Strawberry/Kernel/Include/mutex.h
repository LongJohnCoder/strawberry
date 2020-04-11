// Copyright (c) 2020 Bj�rn Brodtkorb
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


typedef struct Mutex_s
{
	uint32_t lock;
} mutex_s;


//--------------------------------------------------------------------------------------------------//


void mutex_lock(volatile mutex_s* mutex);

void mutex_unlock(volatile mutex_s* mutex);


//--------------------------------------------------------------------------------------------------//


#endif
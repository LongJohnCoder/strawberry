// Copyright (c) 2020 Bj�rn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef SYSCALL_H
#define SYSCALL_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


#define SYSCALL_NOINLINE __attribute__((noinline))


//--------------------------------------------------------------------------------------------------//


// Service numbers
// Service 1 to 10 is reserved for the kernel
// Service 11-255 is free to use

typedef enum
{
	SYSCALL_DELAY = 1,
	SYSCALL_PRINT = 10
} kernel_services;


//--------------------------------------------------------------------------------------------------//


// Prototypes for the services that the kernel provide
void SYSCALL_NOINLINE syscall_print(char* data);

void SYSCALL_NOINLINE syscall_sleep(uint32_t ticks);


//--------------------------------------------------------------------------------------------------//


#endif
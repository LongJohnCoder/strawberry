// Copyright (c) 2020 Bj�rn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef THREAD_H
#define THREAD_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "scheduler.h"


//--------------------------------------------------------------------------------------------------//


void thread_config(void);

thread_s* thread_new(char* thread_name, thread_function thread_func, void* thread_parameter, kernel_thread_priority priority, uint32_t stack_size);


//--------------------------------------------------------------------------------------------------//


#endif
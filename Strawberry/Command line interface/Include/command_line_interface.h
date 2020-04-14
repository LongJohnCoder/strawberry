// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef COMMAND_LINE_INTERFACE_H
#define COMMAND_LINE_INTERFACE_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "scheduler.h"


//--------------------------------------------------------------------------------------------------//


extern struct thread_structure* file_thread;


//--------------------------------------------------------------------------------------------------//


void file_system_command_line_config(void);

void file_system_command_line_print_directory(void);


//--------------------------------------------------------------------------------------------------//


#endif
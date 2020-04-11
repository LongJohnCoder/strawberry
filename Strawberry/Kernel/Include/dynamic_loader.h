// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef DYNAMIC_LOADER_H
#define DYNAMIC_LOADER_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


void dynamic_loader_run(uint32_t* data, uint32_t size);

uint8_t dynamic_loader_check_name(char* data, uint32_t size);

void dynamic_loader_relocate(uint32_t* data);


//--------------------------------------------------------------------------------------------------//


#endif
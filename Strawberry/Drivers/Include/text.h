// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef TEXT_H
#define TEXT_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


void text_to_buffer(char* output, uint32_t* output_size, const char* input, ...);


//--------------------------------------------------------------------------------------------------//


#endif
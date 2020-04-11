// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef CACHE_H
#define CACHE_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


void cache_clean(void);

void cache_clean_addresses(uint32_t* addr, uint32_t size);

void cache_invalidate(void);

void cache_invalidate_addresses(uint32_t* addr, uint32_t size);


//--------------------------------------------------------------------------------------------------//


#endif
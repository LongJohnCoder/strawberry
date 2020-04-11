// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef CORE_H
#define CORE_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


void core_enter_critical_section(uint32_t volatile *atomic);

void core_leave_critical_section(uint32_t volatile *atomic);


//--------------------------------------------------------------------------------------------------//


#define CRITICAL_SECTION_ENTER()				\
{												\
	volatile uint32_t __atomic;					\
	core_enter_critical_section(&__atomic);

	#define CRITICAL_SECTION_LEAVE()			\
	core_leave_critical_section(&__atomic);		\
}


//--------------------------------------------------------------------------------------------------//


#endif
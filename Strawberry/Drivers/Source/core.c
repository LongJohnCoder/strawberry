// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "core.h"
#include "cmsis_gcc.h"
#include "core_cm7.h"


//--------------------------------------------------------------------------------------------------//


void core_enter_critical_section(uint32_t volatile *atomic)
{
	*atomic = __get_PRIMASK();
	__disable_irq();
	__DMB();
}


//--------------------------------------------------------------------------------------------------//


void core_leave_critical_section(uint32_t volatile *atomic)
{
	__DMB();
	__set_PRIMASK(*atomic);
}


//--------------------------------------------------------------------------------------------------//
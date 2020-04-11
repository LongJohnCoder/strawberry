// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "watchdog.h"
#include "core.h"


//--------------------------------------------------------------------------------------------------//


void watchdog_disable(void)
{
	CRITICAL_SECTION_ENTER()
	WDT->WDT_MR |= (1 << WDT_MR_WDDIS_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "flash.h"
#include "core.h"


//--------------------------------------------------------------------------------------------------//


void flash_set_wait_states(uint8_t wait_states)
{
	uint32_t tmp = EFC->EEFC_FMR;
	
	// Write the number of wait states
	tmp &= ~EEFC_FMR_FWS_Msk;
	tmp |= ((wait_states << EEFC_FMR_FWS_Pos) & EEFC_FMR_FWS_Msk);
	
	// Write register
	CRITICAL_SECTION_ENTER()
	EFC->EEFC_FMR = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//
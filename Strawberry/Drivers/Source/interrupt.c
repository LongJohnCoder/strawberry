// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "interrupt.h"
#include "critical_section.h"
#include "core_cm7.h"


//--------------------------------------------------------------------------------------------------//


void interrupt_enable_peripheral_interrupt(IRQn_Type irq_type, interrupt_priority_e irq_priority)
{
	CRITICAL_SECTION_ENTER()
	
	NVIC_DisableIRQ(irq_type);
	NVIC_ClearPendingIRQ(irq_type);
	NVIC_SetPriority(irq_type, irq_priority);
	NVIC_EnableIRQ(irq_type);
	
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void interrupt_disable_peripheral_interrupt(IRQn_Type irq_type)
{
	CRITICAL_SECTION_ENTER()
	
	NVIC_DisableIRQ(irq_type);
	NVIC_ClearPendingIRQ(irq_type);
	
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void interrupt_global_enable(void)
{
	__enable_irq();
}


//--------------------------------------------------------------------------------------------------//


void interrupt_global_disable(void)
{
	__disable_irq();
}


//--------------------------------------------------------------------------------------------------//


void interrupt_set_pending(IRQn_Type irq_type)
{
	NVIC_SetPendingIRQ(irq_type);
}


//--------------------------------------------------------------------------------------------------//
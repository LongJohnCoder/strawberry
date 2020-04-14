// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "systick.h"
#include "critical_section.h"
#include "config.h"


//--------------------------------------------------------------------------------------------------//


void systick_config(void)
{
	CRITICAL_SECTION_ENTER()
	
	SysTick->LOAD = ((SYSTICK_FREQUENCY / 1000) << SysTick_LOAD_RELOAD_Pos);
	SysTick->CTRL =	(1 << SysTick_CTRL_ENABLE_Pos) |
	(1 << SysTick_CTRL_TICKINT_Pos) |
	(1 << SysTick_CTRL_CLKSOURCE_Pos);
	
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void systick_interrupt_enable(void)
{
	CRITICAL_SECTION_ENTER()
	SysTick->CTRL |= (1 << SysTick_CTRL_TICKINT_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void systick_interrupt_disable(void)
{
	CRITICAL_SECTION_ENTER()
	SysTick->CTRL &= ~(1 << SysTick_CTRL_TICKINT_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void systick_disable(void)
{
	CRITICAL_SECTION_ENTER()
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void systick_enable(void)
{
	CRITICAL_SECTION_ENTER()
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void systick_set_reload_value(uint32_t value)
{
	CRITICAL_SECTION_ENTER()
	SysTick->LOAD = value;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void systick_set_counter_value(uint32_t value)
{
	CRITICAL_SECTION_ENTER()
	SysTick->VAL = value;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t systick_get_counter_value(void)
{
	return SysTick->VAL;
}


//--------------------------------------------------------------------------------------------------//


static inline void systick_delay_cycles(uint32_t cycles)
{
	uint8_t n = cycles >> 24;
	uint32_t buf = cycles;
	
	while (n--)
	{
		SysTick->LOAD = 0xffffff;
		SysTick->VAL = 0xffffff;
		
		while (!(SysTick->CTRL & (1 << SysTick_CTRL_COUNTFLAG_Pos)))
		{
			buf -= 0xffffff;
		}
	}
	
	SysTick->LOAD = buf;
	SysTick->VAL = buf;
	
	while (!(SysTick->CTRL & (1 << SysTick_CTRL_COUNTFLAG_Pos)))
	{
		
	}
}


//--------------------------------------------------------------------------------------------------//


static inline uint32_t get_cycles_us(uint32_t us, uint32_t frequency)
{
	return (us * (frequency / 1000) - 1) / 1000 + 1;
}


//--------------------------------------------------------------------------------------------------//


static inline uint32_t get_cycles_ms(uint32_t ms, uint32_t frequency)
{
	return (ms * (frequency / 1000));
}


//--------------------------------------------------------------------------------------------------//


void systick_delay_milliseconds(uint16_t ms)
{
	systick_delay_cycles(get_cycles_ms(ms, SYSTICK_FREQUENCY));
}


//--------------------------------------------------------------------------------------------------//


void systick_delay_microseconds(uint16_t us)
{
	systick_delay_cycles(get_cycles_us(us, SYSTICK_FREQUENCY));
}


//--------------------------------------------------------------------------------------------------//
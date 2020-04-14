// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "gpio.h"
#include "critical_section.h"


//--------------------------------------------------------------------------------------------------//


void gpio_set_pin_function(Pio* hardware, uint8_t pin, gpio_peripheral_function_e func)
{
	//Disable write protection in case
	CRITICAL_SECTION_ENTER()
	hardware->PIO_WPMR = (PIO_WPMR_WPKEY_PASSWD_Val << PIO_WPMR_WPKEY_Pos);
	
	if (hardware == PIOB)
	{
		if ((pin == 4) | (pin == 5) | (pin == 6) | (pin == 7) | (pin == 12))
		{
			MATRIX->CCFG_SYSIO |= (1 << pin);
		}
	}
	
	if (func == PERIPHERAL_FUNCTION_OFF)
	{
		hardware->PIO_PER = (1 << pin);
	}
	else
	{
		if (func & 0b01)
		{

			hardware->PIO_ABCDSR[0] |= (1 << pin);
		}
		else
		{
			hardware->PIO_ABCDSR[0] &= ~(1 << pin);
		}
		
		if (func & 0b10)
		{
			hardware->PIO_ABCDSR[1] |= (1 << pin);
		}
		else
		{
			hardware->PIO_ABCDSR[1] &= ~(1 << pin);
		}
		
		hardware->PIO_PDR = (1 << pin);
	}
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_set_pin_direction_output(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_OER = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_set_pin_direction_input(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_ODR = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_set_pin_value(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_SODR = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_clear_pin_value(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_CODR = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_toogle_pin_value(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	if (hardware->PIO_ODSR & (1 << pin))
	{
		hardware->PIO_CODR = (1 << pin);
	}
	else
	{
		hardware->PIO_SODR = (1 << pin);
	}
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t gpio_get_pin_value_status_register(Pio* hardware)
{
	uint32_t tmp = hardware->PIO_PDSR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void gpio_glitch_filter_enable(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_IFSCDR = (1 << pin);
	hardware->PIO_IFER = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_glitch_filter_disable(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_IFDR = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_debounce_filter_enable(Pio* hardware, uint8_t pin, float ms)
{
	float div_field = ((((float)ms / 0.0625) / 2.0) - 1.0);
	
	CRITICAL_SECTION_ENTER()
	hardware->PIO_IFSCER = (1 << pin);
	hardware->PIO_SCDR = (PIO_SCDR_DIV_Msk & ((uint32_t)div_field << PIO_SCDR_DIV_Pos));
	hardware->PIO_IFER = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_debounce_filter_disable(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_IFDR = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_pullup_enable(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_PUER = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_pullup_disable(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_PUDR = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_pulldown_enable(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_PPDER = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_pulldown_disable(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_PPDDR = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_interrupt_enable(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	gpio_get_interrupt_status_register(hardware);
	hardware->PIO_IER = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_interrupt_disable(Pio* hardware, uint8_t pin)
{
	CRITICAL_SECTION_ENTER()
	hardware->PIO_IDR = (1 << pin);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void gpio_set_interrupt_source(Pio* hardware, uint8_t pin, gpio_interrupt_source_e md)
{
	CRITICAL_SECTION_ENTER()
	if (md == GPIO_STANDARD)
	{
		//Both edge detection
		hardware->PIO_AIMDR = (1 << pin);
	}
	else
	{
		if (md & 0b01)
		{
			hardware->PIO_FELLSR |= (1 << pin);
			hardware->PIO_REHLSR &= ~(1 << pin);
		}
		else
		{
			hardware->PIO_FELLSR &= ~(1 << pin);
			hardware->PIO_REHLSR |= (1 << pin);
		}
		if (md & 0b10)
		{
			hardware->PIO_LSR |= (1 << pin);
			hardware->PIO_ESR &= ~(1 << pin);
		}
		else
		{
			hardware->PIO_ESR |= (1 << pin);
			hardware->PIO_LSR &= ~(1 << pin);
		}
		hardware->PIO_AIMER = (1 << pin);
	}
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t gpio_get_interrupt_status_register(Pio* hardware)
{
	uint32_t tmp = hardware->PIO_ISR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//
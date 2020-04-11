// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "board_button.h"
#include "gpio.h"
#include "interrupt.h"
#include "clock.h"
#include "board_serial.h"
#include "kernel.h"


//--------------------------------------------------------------------------------------------------//


void board_button_config(void)
{
	clock_peripheral_clock_enable(ID_PIOA);
	
	gpio_set_pin_function(PIOA, 11, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_input(PIOA, 11);
	gpio_pullup_enable(PIOA, 11);
	gpio_debounce_filter_enable(PIOA, 11, 10);
	
	gpio_set_interrupt_source(PIOA, 11, GPIO_FALLING_EDGE);
	
	// Enable interrupt if user button reset should be enabled
	//gpio_interrupt_enable(PIOA, 11);
	//interrupt_enable_peripheral_interrupt(PIOA_IRQn, IRQ_LEVEL_3);
}


//--------------------------------------------------------------------------------------------------//


void PIOA_Handler()
{
	gpio_get_pin_value_status_register(PIOA);
	
	//RSTC->RSTC_CR = (RSTC_CR_KEY_PASSWD_Val << RSTC_CR_KEY_Pos) | (1 << RSTC_CR_PROCRST_Pos);
}


//--------------------------------------------------------------------------------------------------//
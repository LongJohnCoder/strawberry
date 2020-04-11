// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "board_sd_card.h"
#include "clock.h"
#include "gpio.h"


//--------------------------------------------------------------------------------------------------//


static void board_sd_card_clock_config(void)
{
	clock_peripheral_clock_enable(ID_HSMCI);
}


//--------------------------------------------------------------------------------------------------//


static void board_sd_card_port_config(void)
{
	gpio_set_pin_function(PIOA, 25, PERIPHERAL_FUNCTION_D);
	gpio_set_pin_function(PIOA, 28, PERIPHERAL_FUNCTION_C);
	
	gpio_set_pin_function(PIOA, 30, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOA, 31, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOA, 26, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOA, 27, PERIPHERAL_FUNCTION_C);
	
	clock_peripheral_clock_enable(ID_PIOC);
	
	gpio_set_pin_function(PIOC, 16, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_input(PIOC, 16);
	gpio_debounce_filter_enable(PIOC, 16, 5);
}


//--------------------------------------------------------------------------------------------------//


board_sd_status board_sd_card_get_status(void)
{
	uint32_t tmp = gpio_get_pin_value_status_register(PIOC);

	if (tmp & (1 << 16))
	{
		return SD_DISCONNECTED;
	}
	return SD_CONNECTED;
}


//--------------------------------------------------------------------------------------------------//


void board_sd_card_config(void)
{
	board_sd_card_clock_config();
	board_sd_card_port_config();
}


//--------------------------------------------------------------------------------------------------//
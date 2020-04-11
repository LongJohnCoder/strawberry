// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "board_led.h"
#include "gpio.h"
#include "syscall.h"


//--------------------------------------------------------------------------------------------------//


void blink_thread(void* arg)
{
	gpio_set_pin_function(PIOC, 8, PERIPHERAL_FUNCTION_OFF);
	gpio_set_pin_direction_output(PIOC, 8);
	
	while (1)
	{
		gpio_clear_pin_value(PIOC, 8);
		syscall_sleep(10);
		gpio_set_pin_value(PIOC, 8);
		syscall_sleep(490);
	}
}


//--------------------------------------------------------------------------------------------------//
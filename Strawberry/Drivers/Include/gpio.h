// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef GPIO_H
#define GPIO_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef enum {
	PERIPHERAL_FUNCTION_A,
	PERIPHERAL_FUNCTION_B,
	PERIPHERAL_FUNCTION_C,
	PERIPHERAL_FUNCTION_D,
	PERIPHERAL_FUNCTION_OFF
} gpio_peripheral_function_e;



typedef enum {
	GPIO_RISING_EDGE,
	GPIO_FALLING_EDGE,
	GPIO_HIGH_LEVEL,
	GPIO_LOW_LEVEL,
	GPIO_STANDARD
} gpio_interrupt_source_e;


//--------------------------------------------------------------------------------------------------//


void gpio_set_pin_function(Pio* hardware, uint8_t pin, gpio_peripheral_function_e func);


//--------------------------------------------------------------------------------------------------//


void gpio_set_pin_direction_output(Pio* hardware, uint8_t pin);

void gpio_set_pin_direction_input(Pio* hardware, uint8_t pin);


//--------------------------------------------------------------------------------------------------//


void gpio_set_pin_value(Pio* hardware, uint8_t pin);

void gpio_clear_pin_value(Pio* hardware, uint8_t pin);

void gpio_toogle_pin_value(Pio* hardware, uint8_t pin);


//--------------------------------------------------------------------------------------------------//


void gpio_glitch_filter_enable(Pio* hardware, uint8_t pin);

void gpio_glitch_filter_disable(Pio* hardware, uint8_t pin);

void gpio_debounce_filter_enable(Pio* hardware, uint8_t pin, float ms);

void gpio_debounce_filter_disable(Pio* hardware, uint8_t pin);


//--------------------------------------------------------------------------------------------------//


void gpio_pullup_enable(Pio* hardware, uint8_t pin);

void gpio_pullup_disable(Pio* hardware, uint8_t pin);

void gpio_pulldown_enable(Pio* hardware, uint8_t pin);

void gpio_pulldown_disable(Pio* hardware, uint8_t pin);


//--------------------------------------------------------------------------------------------------//


uint32_t gpio_get_pin_value_status_register(Pio* hardware);


//--------------------------------------------------------------------------------------------------//


void gpio_interrupt_enable(Pio* hardware, uint8_t pin);

void gpio_interrupt_disable(Pio* hardware, uint8_t pin);

void gpio_set_interrupt_source(Pio* hardware, uint8_t pin, gpio_interrupt_source_e md);

uint32_t gpio_get_interrupt_status_register(Pio* hardware);


//--------------------------------------------------------------------------------------------------//


#endif
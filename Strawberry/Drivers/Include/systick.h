// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef SYSTICK_H
#define SYSTICK_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


void systick_config(void);

void systick_interrupt_enable(void);

void systick_interrupt_disable(void);

void systick_disable(void);

void systick_enable(void);

void systick_set_reload_value(uint32_t value);

void systick_set_counter_value(uint32_t value);

uint32_t systick_get_counter_value(void);


//--------------------------------------------------------------------------------------------------//


void systick_delay_milliseconds(const uint16_t ms);

void systick_delay_microseconds(const uint16_t us);


//--------------------------------------------------------------------------------------------------//


#endif
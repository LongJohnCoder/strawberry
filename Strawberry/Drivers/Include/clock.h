// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef CLOCK_H
#define CLOCK_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	CLOCK_SOURCE_CRYSTAL,
	CLOCK_SOURCE_RC
} clock_source_e;



typedef enum
{
	CLOCK_MASTER_CLOCK_DIVISION_NO_DIV,
	CLOCK_MASTER_CLOCK_DIVISION_DIV_2,
	CLOCK_MASTER_CLOCK_DIVISION_DIV_4,
	CLOCK_MASTER_CLOCK_DIVISION_DIV_3
} clock_master_clock_division_e;



typedef enum
{
	CLOCK_MASTER_CLOCK_PRESCALER_1,
	CLOCK_MASTER_CLOCK_PRESCALER_2,
	CLOCK_MASTER_CLOCK_PRESCALER_4,
	CLOCK_MASTER_CLOCK_PRESCALER_8,
	CLOCK_MASTER_CLOCK_PRESCALER_16,
	CLOCK_MASTER_CLOCK_PRESCALER_32,
	CLOCK_MASTER_CLOCK_PRESCALER_64,
	CLOCK_MASTER_CLOCK_PRESCALER_3
} clock_master_clock_prescaler_e;



typedef enum
{
	CLOCK_MASTER_CLOCK_SOURCE_SLOW_CLOCK,
	CLOCK_MASTER_CLOCK_SOURCE_MAIN_CLOCK,
	CLOCK_MASTER_CLOCK_SOURCE_PLLA_CLOCK
} clock_master_clock_source_e;



typedef enum
{
	CLOCK_SOURCE_FREQUENCY_12_MHZ,
	CLOCK_SOURCE_FREQUENCY_16_MHZ
} clock_source_frequency_e;



typedef enum
{
	CLOCK_USB_SOURCE_PLL,
	CLOCK_USB_SOURCE_USB_PLL
} clock_usb_source_e;



typedef enum
{
	PROGRAMMABLE_CLOCK_0,
	PROGRAMMABLE_CLOCK_1,
	PROGRAMMABLE_CLOCK_2,
	PROGRAMMABLE_CLOCK_3,
	PROGRAMMABLE_CLOCK_4,
	PROGRAMMABLE_CLOCK_5,
	PROGRAMMABLE_CLOCK_6,
	PROGRAMMABLE_CLOCK_7
} clock_programmable_clock_e;


//--------------------------------------------------------------------------------------------------//


void clock_sources_config(clock_source_e clock_oscillator, uint8_t startup_time);

void clock_main_clock_config(clock_source_e clock_oscillator);

void clock_pll_config(uint16_t multiplication_factor, uint8_t division_factor, uint8_t startup_time);

void clock_master_clock_config(clock_master_clock_source_e source, clock_master_clock_prescaler_e prescaler, clock_master_clock_division_e division);

uint32_t clock_get_cpu_frequency(void);


//--------------------------------------------------------------------------------------------------//


void clock_usb_pll_config(clock_source_frequency_e source_frequency, uint8_t startup_time, uint8_t divider);

void clock_usb_config(clock_usb_source_e usb_source, uint8_t division);

void clock_usb_full_speed_enable(void);

void clock_usb_full_speed_disable(void);


//--------------------------------------------------------------------------------------------------//


void clock_programmable_clocks_enable(clock_programmable_clock_e clock, clock_master_clock_source_e source, uint8_t prescaler);

void clock_programmable_clock_disable(clock_programmable_clock_e clock);


//--------------------------------------------------------------------------------------------------//


void clock_peripheral_clock_enable(uint8_t id_peripheral);

void clock_peripheral_clock_disable(uint8_t id_peripheral);


//--------------------------------------------------------------------------------------------------//


#endif
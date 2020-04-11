// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef TIMER_H
#define TIMER_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


#define TIMER_INTERRUPT_EXTERNAL_TRIGGER	TC_IER_ETRGS_Msk
#define TIMER_INTERRUPT_B_LOADING			TC_IER_LDRBS_Msk
#define TIMER_INTERRUPT_A_LOADING			TC_IER_LDRAS_Msk
#define TIMER_INTERRUPT_C_COMPARE			TC_IER_CPCS_Msk
#define TIMER_INTERRUPT_B_COMPARE			TC_IER_CPBS_Msk
#define TIMER_INTERRUPT_A_COMPARE			TC_IER_CPAS_Msk
#define TIMER_INTERRUPT_LOAD_OVERRUN		TC_IER_LOVRS_Msk
#define TIMER_INTERRUPT_COUTER_OVERFLOW		TC_IER_COVFS_Msk


#define TC0_CHANNEL0_IRQn TC0_IRQn
#define TC0_CHANNEL1_IRQn TC1_IRQn
#define TC0_CHANNEL2_IRQn TC2_IRQn


#define TC1_CHANNEL0_IRQn TC3_IRQn
#define TC1_CHANNEL1_IRQn TC4_IRQn
#define TC1_CHANNEL2_IRQn TC5_IRQn


#define TC2_CHANNEL0_IRQn TC6_IRQn
#define TC2_CHANNEL1_IRQn TC7_IRQn
#define TC2_CHANNEL2_IRQn TC8_IRQn


#define TC3_CHANNEL0_IRQn TC9_IRQn
#define TC3_CHANNEL1_IRQn TC10_IRQn
#define TC3_CHANNEL2_IRQn TC11_IRQn


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	TIMER_CHANNEL_0,
	TIMER_CHANNEL_1,
	TIMER_CHANNEL_2
} timer_channel_e;



typedef enum
{
	TIMER_CLOCK_PCK,
	TIMER_CLOCK_MCK_DIV_8,
	TIMER_CLOCK_MCK_DIV_32,
	TIMER_CLOCK_MCK_DIV_128 = 3,
	TIMER_CLOCK_SLCK,
	TIMER_CLOCK_X1,
	TIMER_CLOCK_X2,
	TIMER_CLOCK_X3
} timer_clock_source_e;



typedef enum
{
	TIMER_INCREMENT_RISING_EDGE,
	TIMER_INCREMENT_FALLING_EDGE
} timer_clock_invert_e;

typedef enum
{
	TIMER_NONE,
	TIMER_RISING_EDGE,
	TIMER_FALLING_EDGE,
	TIMER_BOTH_EDGES
} timer_edge_e;



typedef enum
{
	TIMER_CAPTURE_MODE,
	TIMER_WAVEFORM_MODE
} timer_waveform_mode_e;


//--------------------------------------------------------------------------------------------------//


void timer_clock_enable(Tc* hardware, timer_channel_e channel);

void timer_clock_disable(Tc* hardware, timer_channel_e channel);

void timer_software_trigger(Tc* hardware, timer_channel_e channel);


//--------------------------------------------------------------------------------------------------//


void timer_write_protection_disable(Tc* hardware);

void timer_write_protection_enable(Tc* hardware);


//--------------------------------------------------------------------------------------------------//


uint32_t timer_read_counter_value(Tc* hardware, timer_channel_e channel);

uint32_t timer_read_compare_a(Tc* hardware, timer_channel_e channel);

uint32_t timer_read_compare_b(Tc* hardware, timer_channel_e channel);

uint32_t timer_read_compare_c(Tc* hardware, timer_channel_e channel);

void timer_set_compare_a(Tc* hardware, timer_channel_e channel, uint32_t value);

void timer_set_compare_b(Tc* hardware, timer_channel_e channel, uint32_t value);

void timer_set_compare_c(Tc* hardware, timer_channel_e channel, uint32_t value);


//--------------------------------------------------------------------------------------------------//


uint32_t timer_read_interrupt_status(Tc* hardware, timer_channel_e channel);

uint32_t timer_read_interrupt_mask(Tc* hardware, timer_channel_e channel);

void timer_interrupt_enable(Tc* hardware, timer_channel_e channel, uint32_t interrupt_mask);

void timer_interrupt_disable(Tc* hardware, timer_channel_e channel, uint32_t interrupt_mask);


//--------------------------------------------------------------------------------------------------//


void timer_capture_mode_config(	Tc* hardware,
								timer_channel_e channel,
								timer_edge_e register_b_loading,
								timer_edge_e register_a_loading,
								timer_waveform_mode_e wavform_mode,
								uint8_t reset_on_c_compare,
								uint8_t clock_stop_on_b_loading,
								uint8_t clock_disable_on_b_loading,
								timer_clock_invert_e clock_invert,
								timer_clock_source_e clock_source);


//--------------------------------------------------------------------------------------------------//


#endif
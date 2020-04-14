// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "timer.h"
#include "critical_section.h"


//--------------------------------------------------------------------------------------------------//


void timer_clock_enable(Tc* hardware, timer_channel_e channel)
{
	CRITICAL_SECTION_ENTER()
	hardware->TcChannel[channel].TC_CCR = (1 << TC_CCR_CLKEN_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void timer_clock_disable(Tc* hardware, timer_channel_e channel)
{
	CRITICAL_SECTION_ENTER()
	hardware->TcChannel[channel].TC_CCR = (1 << TC_CCR_CLKDIS_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void timer_software_trigger(Tc* hardware, timer_channel_e channel)
{
	CRITICAL_SECTION_ENTER()
	hardware->TcChannel[channel].TC_CCR = (1 << TC_CCR_SWTRG_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void timer_write_protection_disable(Tc* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->TC_WPMR = (TC_WPMR_WPKEY_PASSWD_Val << TC_WPMR_WPKEY_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void timer_write_protection_enable(Tc* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->TC_WPMR = (TC_WPMR_WPKEY_PASSWD_Val << TC_WPMR_WPKEY_Pos) | (1 << TC_WPMR_WPEN_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t timer_read_counter_value(Tc* hardware, timer_channel_e channel)
{
	uint32_t tmp = hardware->TcChannel[channel].TC_CV;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t timer_read_compare_a(Tc* hardware, timer_channel_e channel)
{
	uint32_t tmp = hardware->TcChannel[channel].TC_RA;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t timer_read_compare_b(Tc* hardware, timer_channel_e channel)
{
	uint32_t tmp = hardware->TcChannel[channel].TC_RB;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t timer_read_compare_c(Tc* hardware, timer_channel_e channel)
{
	uint32_t tmp = hardware->TcChannel[channel].TC_RC;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void timer_set_compare_a(Tc* hardware, timer_channel_e channel, uint32_t value)
{
	CRITICAL_SECTION_ENTER()
	hardware->TcChannel[channel].TC_RA = value;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void timer_set_compare_b(Tc* hardware, timer_channel_e channel, uint32_t value)
{
	CRITICAL_SECTION_ENTER()
	hardware->TcChannel[channel].TC_RB = value;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void timer_set_compare_c(Tc* hardware, timer_channel_e channel, uint32_t value)
{
	CRITICAL_SECTION_ENTER()
	hardware->TcChannel[channel].TC_RC = value;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t timer_read_interrupt_status(Tc* hardware, timer_channel_e channel)
{
	uint32_t tmp = hardware->TcChannel[channel].TC_SR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t timer_read_interrupt_mask(Tc* hardware, timer_channel_e channel)
{
	uint32_t tmp = hardware->TcChannel[channel].TC_IMR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void timer_interrupt_enable(Tc* hardware, timer_channel_e channel, uint32_t interrupt_mask)
{
	CRITICAL_SECTION_ENTER()
	hardware->TcChannel[channel].TC_IER = interrupt_mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void timer_interrupt_disable(Tc* hardware, timer_channel_e channel, uint32_t interrupt_mask)
{
	CRITICAL_SECTION_ENTER()
	hardware->TcChannel[channel].TC_IDR = interrupt_mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


// This configuration is for the capture mode

void timer_capture_mode_config(	Tc* hardware,
								timer_channel_e channel,
								timer_edge_e register_b_loading,
								timer_edge_e register_a_loading,
								timer_waveform_mode_e wavform_mode,
								uint8_t reset_on_c_compare,
								uint8_t clock_stop_on_b_loading,
								uint8_t clock_disable_on_b_loading,
								timer_clock_invert_e clock_invert,
								timer_clock_source_e clock_source)
{
	uint32_t tmp =	(register_b_loading << TC_CMR_CAPTURE_LDRB_Pos) |
					(register_a_loading << TC_CMR_CAPTURE_LDRA_Pos) |
					(wavform_mode << TC_CMR_WAVE_Pos) |
					(reset_on_c_compare << TC_CMR_CAPTURE_CPCTRG_Pos) | 
					(clock_stop_on_b_loading << TC_CMR_CAPTURE_LDBSTOP_Pos) |
					(clock_disable_on_b_loading << TC_CMR_CAPTURE_LDBDIS_Pos) |
					(clock_invert << TC_CMR_CLKI_Pos) |
					(clock_source << TC_CMR_TCCLKS_Pos);
	
	CRITICAL_SECTION_ENTER()
	hardware->TcChannel[channel].TC_CMR = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//
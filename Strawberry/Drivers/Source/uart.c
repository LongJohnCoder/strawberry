// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "uart.h"
#include "critical_section.h"


//--------------------------------------------------------------------------------------------------//

void uart_write_protection_enable(Uart* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_WPMR = (UART_WPMR_WPKEY_PASSWD_Val << UART_WPMR_WPKEY_Pos) | (1 << UART_WPMR_WPEN_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_write_protection_disable(Uart* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_WPMR = (UART_WPMR_WPKEY_PASSWD_Val << UART_WPMR_WPKEY_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_transmitter_enable(Uart* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_CR = (1 << UART_CR_TXEN_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_transmitter_disable(Uart* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_CR = (1 << UART_CR_TXDIS_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_transmitter_reset(Uart* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_CR = (1 << UART_CR_RSTRX_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_receiver_enable(Uart* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_CR = (1 << UART_CR_RXEN_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_receiver_disable(Uart* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_CR = (1 << UART_CR_RXDIS_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_receiver_reset(Uart* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_CR = (1 << UART_CR_RSTRX_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_mode_config(Uart* hardware, uart_parity_e parity, uart_channel_mode_e channel_mode, uart_source_clock_e source_clock, uint8_t filter_enable)
{
	uint32_t tmp = (parity << UART_MR_PAR_Pos) | (channel_mode << UART_MR_CHMODE_Pos) | (source_clock << UART_MR_BRSRCCK_Pos) | (filter_enable << UART_MR_FILTER);
	
	CRITICAL_SECTION_ENTER()
	hardware->UART_MR = tmp;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_set_buad_rate(Uart* hardware, uint32_t baud_rate)
{
	uint32_t source_clock;
	
	// The baud rate is given from the following formula
	// CD = source_clock / 16 / baud_rate
	// Where the source clock can either be MCK or PCK
	
	uint32_t mode_register = hardware->UART_MR;
	
	if (mode_register & (1 << UART_MR_BRSRCCK_Pos))
	{
		// PCK is used to derive the UART clock
		//
		// This is not supported yet...
		source_clock = 0;
	}
	else
	{
		// MCK is used to derive the UART clock
		source_clock = 150000000;
	}
	
	uint16_t cd_value = (uint16_t)(source_clock / 16 / baud_rate);
	
	// Write the CD value to the register
	CRITICAL_SECTION_ENTER()
	hardware->UART_BRGR = cd_value;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t uart_read_interrupt_status(Uart* hardware)
{
	uint32_t tmp = hardware->UART_SR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t uart_read_interrupt_mask(Uart* hardware)
{
	uint32_t tmp = hardware->UART_IMR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void uart_interrupt_enable(Uart* hardware, uint32_t interrupt_mask)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_IER = interrupt_mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void uart_interrupt_disable(Uart* hardware, uint32_t interrupt_mask)
{
	CRITICAL_SECTION_ENTER()
	hardware->UART_IDR = interrupt_mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint8_t uart_read(Uart* hardware)
{
	uint8_t tmp = hardware->UART_RHR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void uart_write(Uart* hardware, uint8_t data)
{
	while (!(hardware->UART_SR & (1 << UART_SR_TXEMPTY_Pos)))
	{
		// Wait for the buffer to be empty
	}
	
	CRITICAL_SECTION_ENTER()
	hardware->UART_THR = data;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//
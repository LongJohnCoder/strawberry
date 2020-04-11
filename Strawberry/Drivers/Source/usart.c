// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "usart.h"
#include "core.h"


//--------------------------------------------------------------------------------------------------//


void usart_mode_config(Usart* hardware, usart_stop_bits_e sp, usart_parity_e pb, usart_synchronous_mode_e sm, uint16_t cd_value)
{
	// Disable USART write protection
	CRITICAL_SECTION_ENTER()
	hardware->US_WPMR = (US_WPMR_WPKEY_Msk & (US_WPMR_WPKEY_PASSWD_Val << US_WPMR_WPKEY_Pos));
	CRITICAL_SECTION_LEAVE()

	// Reset registers that can cause trouble after reset
	CRITICAL_SECTION_ENTER()
	hardware->US_MR = 0x0;
	hardware->US_RTOR = 0x0;
	hardware->US_TTGR = 0x0;
	CRITICAL_SECTION_LEAVE()
	
	// Reset receiver and transmitter
	CRITICAL_SECTION_ENTER()
	hardware->US_CR =	(1 << US_CR_RSTRX_Pos) | (1 << US_CR_RSTTX_Pos) | (1 << US_CR_TXDIS_Pos) | (1 << US_CR_RXDIS_Pos);
	CRITICAL_SECTION_LEAVE()
	
	// Reset status bits
	CRITICAL_SECTION_ENTER()
	hardware->US_CR |= (1 << US_CR_RSTSTA_Pos);
	CRITICAL_SECTION_LEAVE()
	
	// Turn off DTR and RTS
	CRITICAL_SECTION_ENTER()
	hardware->US_CR |= (1 << US_CR_USART_RTSDIS_Pos);
	CRITICAL_SECTION_LEAVE()
	
	// Configure USART mode x16 oversampling
	uint32_t tmp_reg =	((uint8_t)sp << US_MR_USART_NBSTOP_Pos) |
						((uint8_t)pb << US_MR_USART_PAR_Pos) |
						((uint8_t)sm << US_MR_USART_SYNC_Pos) |
						(1 << US_MR_USART_INACK_Pos) |
						(US_MR_CHRL_8_BIT_Val << US_MR_CHRL_Pos);
	
	CRITICAL_SECTION_ENTER()
	hardware->US_MR = tmp_reg;
	CRITICAL_SECTION_LEAVE()
	
	// Configure USART baud rate register (OVER = 0)
	CRITICAL_SECTION_ENTER()
	hardware->US_BRGR = (US_BRGR_CD_Msk & (cd_value << US_BRGR_CD_Pos));
	CRITICAL_SECTION_LEAVE()
	
	// Enable receiver and transmitter
	hardware->US_CR |= (1 << US_CR_TXEN_Pos) | (1 << US_CR_RXEN_Pos);
}


//--------------------------------------------------------------------------------------------------//


void usart_interrupt_enable(Usart* hardware, uint32_t interrupt_mask)
{
	CRITICAL_SECTION_ENTER()
	hardware->US_IER = interrupt_mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usart_interrupt_disable(Usart* hardware, uint32_t interrupt_mask)
{
	CRITICAL_SECTION_ENTER()
	hardware->US_IDR = interrupt_mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void usart_write(Usart* hardware, char data)
{
	hardware->US_THR = data;
	
	//wait for data to be shifted out
	while (!(hardware->US_CSR & (1 << US_CSR_TXRDY_Pos)))
	{
		
	}
}


//--------------------------------------------------------------------------------------------------//


char usart_read(Usart* hardware)
{
	uint8_t data = (uint8_t)hardware->US_RHR;

	return data;
}


//--------------------------------------------------------------------------------------------------//


uint32_t usart_get_status_register(Usart* hardware)
{
	uint32_t status = hardware->US_CSR;
	
	return status;
}


//--------------------------------------------------------------------------------------------------//
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef UART_H
#define UART_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


// The interrupts are as follows
//
// Comparison		- the received character matches the comparison criteria
// TX empty			- there are no characters in the shift register
// Parity error		- at least one parity error has occurred since last time
// Framing error	- at least one framing error has occurred since last time
// Overrun error	- at least one overrun error has occurred since the last time
// TX ready			- no character has been written to THR but not yet shifted out
// RX ready			- at least one complete character has been received  

#define UART_INTERRUPT_COMPARISON		UART_IER_CMP_Msk
#define UART_INTERRUPT_TX_EMPTY			UART_IER_TXEMPTY_Msk
#define UART_INTERRUPT_PARITY_ERROR		UART_IER_PARE_Msk
#define UART_INTERRUPT_FRAMING_ERROR	UART_IER_FRAME_Msk
#define UART_INTERRUPT_OVERRUN_ERROR	UART_IER_OVRE_Msk
#define UART_INTERRUPT_TX_READY			UART_IER_TXRDY_Msk
#define UART_INTERRUPT_RX_READY			UART_IER_RXRDY_Msk


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	UART_PARITY_EVEN,
	UART_PARITY_ODD,
	UART_PARITY_SPACE,
	UART_PARITY_MARK,
	UART_PARITY_NO
} uart_parity_e;



typedef enum
{
	UART_CH_MOD_NORMAL,
	UART_CH_MOD_AUTOMATIC,
	UART_CH_MOD_LOCAL_LOOPBACK,
	UART_CH_MOD_REMOTE_LOOPBACK
} uart_channel_mode_e;



typedef enum
{
	UART_SOURCE_CLOCK_MCK,
	UART_SOURCE_CLOCK_PCK
} uart_source_clock_e;


//--------------------------------------------------------------------------------------------------//


void uart_write_protection_enable(Uart* hardware);

void uart_write_protection_disable(Uart* hardware);


//--------------------------------------------------------------------------------------------------//


void uart_transmitter_enable(Uart* hardware);

void uart_transmitter_disable(Uart* hardware);

void uart_transmitter_reset(Uart* hardware);

void uart_receiver_enable(Uart* hardware);

void uart_receiver_disable(Uart* hardware);

void uart_receiver_reset(Uart* hardware);


//--------------------------------------------------------------------------------------------------//


void uart_mode_config(Uart* hardware, uart_parity_e parity, uart_channel_mode_e channel_mode, uart_source_clock_e source_clock, uint8_t filter_enable);

void uart_set_buad_rate(Uart* hardware, uint32_t baud_rate);


//--------------------------------------------------------------------------------------------------//


uint32_t uart_read_interrupt_status(Uart* hardware);

uint32_t uart_read_interrupt_mask(Uart* hardware);

void uart_interrupt_enable(Uart* hardware, uint32_t interrupt_mask);

void uart_interrupt_disable(Uart* hardware, uint32_t interrupt_mask);


//--------------------------------------------------------------------------------------------------//


uint8_t uart_read(Uart* hardware);

void uart_write(Uart* hardware, uint8_t data);


//--------------------------------------------------------------------------------------------------//


#endif
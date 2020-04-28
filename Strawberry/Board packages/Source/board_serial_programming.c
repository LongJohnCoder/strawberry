// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "board_serial_programming.h"
#include "clock.h"
#include "gpio.h"
#include "usart.h"
#include "interrupt.h"
#include "config.h"
#include "dma.h"
#include "timer.h"


//--------------------------------------------------------------------------------------------------//


#include <stdio.h>
#include <stdarg.h>


//--------------------------------------------------------------------------------------------------//


static char serial_print_buffer[256];

static const unsigned s_value[] = {1000000000u, 100000000u, 10000000u, 1000000u, 100000u, 10000u, 1000u, 100u, 10u, 1u};
	

//--------------------------------------------------------------------------------------------------//


void board_serial_programming_dma_config(void);


//--------------------------------------------------------------------------------------------------//


static void board_serial_x_port_config(void)
{
	gpio_set_pin_function(BOARD_SERIAL_PROGRAMMING_TX_PORT, BOARD_SERIAL_PROGRAMMING_TX_PIN, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(BOARD_SERIAL_PROGRAMMING_RX_PORT, BOARD_SERIAL_PROGRAMMING_RX_PIN, PERIPHERAL_FUNCTION_C);
}


//--------------------------------------------------------------------------------------------------//


static void board_serial_x_clock_config(void)
{
	clock_peripheral_clock_enable(ID_USART0);
}


//--------------------------------------------------------------------------------------------------//


static void board_serial_x_mode_config(void)
{
	usart_mode_config(USART0, USART_STOP_BIT_1, USART_PARITY_NO, USART_MODE_ASYNCHRONOUS, BOARD_SERIAL_PROGRAMMING_CD_VALUE);
	
	usart_interrupt_enable(USART0, USART_IRQ_RX_READY);
	interrupt_enable_peripheral_interrupt(USART0_IRQn, IRQ_LEVEL_4);
}


//--------------------------------------------------------------------------------------------------//


void board_serial_programming_config(void)
{
	board_serial_x_port_config();
	board_serial_x_clock_config();
	board_serial_x_mode_config();
	
	board_serial_programming_dma_config();
}


//--------------------------------------------------------------------------------------------------//


void board_serial_programming_write(char data)
{
	usart_write(USART0, data);
}


//--------------------------------------------------------------------------------------------------//


static int printu_override(char *s, unsigned u)
{
	char tmp_buf[12];
	int  i, n = 0;
	int  m;

	if (u == 0) {
		*s = '0';
		return 1;
	}

	for (i = 0; i < 10; i++) {
		for (m = 0; m < 10; m++) {
			if (u >= s_value[i]) {
				u -= s_value[i];
				} else {
				break;
			}
		}
		tmp_buf[i] = m + '0';
	}
	for (i = 0; i < 10; i++) {
		if (tmp_buf[i] != '0') {
			break;
		}
	}
	for (; i < 10; i++) {
		*s++ = tmp_buf[i];
		n++;
	}
	return n;
}


//--------------------------------------------------------------------------------------------------//


void board_serial_programming_print(char* data, ...)
{
	char* start = serial_print_buffer;
	char* s = serial_print_buffer;

	int     n = 0;
	va_list ap;
	va_start(ap, data);
	while (*data) {
		if (*data != '%') {
			*s = *data;
			s++;
			data++;
			n++;
			} else {
			data++;
			switch (*data) {
				case 'c': {
					char valch = va_arg(ap, int);
					*s         = valch;
					s++;
					data++;
					n++;
					break;
				}
				case 'd': {
					int vali = va_arg(ap, int);
					int nc;

					if (vali < 0) {
						*s++ = '-';
						n++;
						nc = printu_override(s, -vali);
						} else {
						nc = printu_override(s, vali);
					}

					s += nc;
					n += nc;
					data++;
					break;
				}
				case 'u': {
					unsigned valu = va_arg(ap, unsigned);
					int      nc   = printu_override(s, valu);
					n += nc;
					s += nc;
					data++;
					break;
				}
				case 's': {
					char *vals = va_arg(ap, char *);
					while (*vals) {
						*s = *vals;
						s++;
						vals++;
						n++;
					}
					data++;
					break;
				}
				default:
				*s = *data;
				s++;
				data++;
				n++;
			}
		}
	}
	va_end(ap);
	*s = 0;

	board_serial_programming_dma_print(start);
}


//--------------------------------------------------------------------------------------------------//


void board_serial_programming_write_percent(char first, char second)
{
	board_serial_programming_print("%d.%d", first, second);
	board_serial_programming_dma_print("%");
}


//--------------------------------------------------------------------------------------------------//


typedef struct
{
	char data[SERIAL_DMA_BUFFER_SIZE];
	uint16_t position;
	
	uint8_t dma_active;
	
} serial_programming_buffer;



static volatile serial_programming_buffer prog_buffer_a;
static volatile serial_programming_buffer prog_buffer_b;


static volatile serial_programming_buffer* prog_current_buffer;
static volatile serial_programming_buffer* prog_dma_buffer;


//--------------------------------------------------------------------------------------------------//


// This function gets called when a DMA serial transaction has completed. The dma_buffer pointer
// points to data that is successfully transmitted. 

void board_serial_programming_dma_callback(uint8_t channel)
{
	prog_dma_buffer->dma_active = 0;
	prog_dma_buffer->position = 0;
}


//--------------------------------------------------------------------------------------------------//


// This function will set up a DMA serial transaction 

void board_serial_programming_dma_flush_buffer(char* source_buffer, uint32_t size)
{	
	dma_microblock_transaction_descriptor dma_desc;

	
	// We configure the channel
	dma_desc.burst_size = DMA_BURST_SIZE_SINGLE;
	dma_desc.chunk_size = DMA_CHUNK_SIZE_1;
	dma_desc.data_width = DMA_DATA_WIDTH_BYTE;

	dma_desc.destination_adressing_mode = DMA_DEST_ADDRESSING_FIXED;
	dma_desc.destination_bus_interface = DMA_AHB_INTERFACE_1;
	dma_desc.destination_pointer = (uint32_t *)&(((Usart *)(USART0))->US_THR);

	dma_desc.memory_fill = DMA_MEMORY_FILL_OFF;
	dma_desc.peripheral_id = XDMAC_CC_PERID_USART0_TX_Val;

	dma_desc.size = size;

	dma_desc.channel = BOARD_SERIAL_PROGRAMMING_DMA_CHANNEL;

	dma_desc.source_addressing_mode = DMA_SOURCE_ADDRESSING_INCREMENTED;
	dma_desc.source_bus_inteface = DMA_AHB_INTERFACE_0;
	dma_desc.source_pointer = (uint32_t *)source_buffer;

	dma_desc.synchronization = DMA_SYNC_MEMORY_TO_PERIPHERAL;
	dma_desc.transfer_type = DMA_TRANSFER_TYPE_PERIPHERAL_TRANSFER;
	dma_desc.trigger = DMA_TRIGGER_HARDWARE;
	
	
	
	// Set a callback handler
	dma_channel_set_callback(BOARD_SERIAL_PROGRAMMING_DMA_CHANNEL, board_serial_programming_dma_callback);


	// The serial buffer that is flushed might be located in the cache. Therefore
	// we have to clean the cache lines that corresponds to the serial buffer. That
	// means that the cache data is written back to memory.
	uint32_t* cache_addr = (uint32_t *)((uint32_t)source_buffer & ~((uint32_t)32));

	// Cache can only be cleaned at 32-bytes alignment
	SCB_CleanDCache_by_Addr(cache_addr, size + 32);
	
	SCB_CleanDCache();

	prog_dma_buffer->dma_active = 1;

	// Start the transfer
	dma_setup_transaction(XDMAC, &dma_desc);
}


//--------------------------------------------------------------------------------------------------//


// The DMA serial interface is a double buffered serial interface
// which allows printing to screen with no processor overhead.
//
// The serial buffer will be flushed when enough bytes are ready to be transmitted,
// or a timer overflow has occurred.

void board_serial_programming_dma_config(void)
{
	// Configure the timer interface (timer 0 channel 0) to overflow after 10 ms
	clock_peripheral_clock_enable(ID_TC0_CHANNEL1);
	
	timer_write_protection_disable(TC0);
	timer_capture_mode_config(TC0, TIMER_CHANNEL_1, TIMER_NONE, TIMER_NONE, TIMER_CAPTURE_MODE, 1, 0, 0, TIMER_INCREMENT_RISING_EDGE, TIMER_CLOCK_MCK_DIV_128);
	timer_set_compare_c(TC0, TIMER_CHANNEL_1, 58595);
	timer_interrupt_enable(TC0, TIMER_CHANNEL_1, TIMER_INTERRUPT_C_COMPARE);
	
	interrupt_enable_peripheral_interrupt(TC1_IRQn, IRQ_LEVEL_6);
	
	prog_dma_buffer = &prog_buffer_b;
	prog_current_buffer = &prog_buffer_a;
	
	prog_current_buffer->dma_active = 0;
	prog_dma_buffer->dma_active = 0;
	
	prog_current_buffer->position = 0;
	prog_dma_buffer->position = 0;
}


//--------------------------------------------------------------------------------------------------//


void board_serial_programming_timer_start(void)
{
	timer_clock_enable(TC0, TIMER_CHANNEL_1);
	timer_software_trigger(TC0, TIMER_CHANNEL_1);
}


//--------------------------------------------------------------------------------------------------//


void board_serial_programming_timer_stop(void)
{
	timer_clock_disable(TC0, TIMER_CHANNEL_1);
}


//--------------------------------------------------------------------------------------------------//


void board_serial_programming_dma_switch_buffers(void)
{
	// We must check that the DMA buffer is ready
	while (prog_dma_buffer->dma_active)
	{

	}
	
	// After the DMA transaction is complete we switch the buffers
	serial_programming_buffer* tmp = (serial_programming_buffer *)prog_dma_buffer;
	
	prog_dma_buffer = prog_current_buffer;
	prog_current_buffer = tmp;
}


//--------------------------------------------------------------------------------------------------//


void board_serial_programming_dma_print(char* data)
{
	while (*data)
	{
		prog_current_buffer->data[prog_current_buffer->position] = *data;
		
		prog_current_buffer->position++;
		data++;
		
		if (prog_current_buffer->position == SERIAL_DMA_BUFFER_SIZE)
		{
			// We have to disable the timer here
			board_serial_programming_timer_stop();
			
			// The buffer is filled up
			board_serial_programming_dma_switch_buffers();
			
			// Start flushing the DMA buffer
			board_serial_programming_dma_flush_buffer((char *)(prog_dma_buffer->data), prog_dma_buffer->position);
		}
	}
	
	/*
	// The buffer is filled up
	board_serial_dma_switch_buffers();
	
	// Start flushing the DMA buffer
	board_serial_dma_flush_buffer(dma_buffer->data, dma_buffer->position);
	*/
	
	board_serial_programming_timer_start();
}


//--------------------------------------------------------------------------------------------------//


void TC1_Handler()
{
	timer_read_interrupt_status(TC0, TIMER_CHANNEL_1);
	
	// We have to disable the timer here
	board_serial_programming_timer_stop();
	
	// The buffer is filled up
	board_serial_programming_dma_switch_buffers();
	
	// Start flushing the DMA buffer
	board_serial_programming_dma_flush_buffer((char *)(prog_dma_buffer->data), prog_dma_buffer->position);
}


//--------------------------------------------------------------------------------------------------//
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "board_serial.h"
#include "config.h"
#include "usart.h"
#include "clock.h"
#include "gpio.h"
#include "interrupt.h"
#include "dma.h"
#include "timer.h"


//--------------------------------------------------------------------------------------------------//


#include <stdio.h>
#include <stdarg.h>


//--------------------------------------------------------------------------------------------------//


static char serial_print_buffer[SERIAL_PRINTF_BUFFER_SIZE];

static const unsigned s_value[] = {1000000000u, 100000000u, 10000000u, 1000000u, 100000u, 10000u, 1000u, 100u, 10u, 1u};

	
//--------------------------------------------------------------------------------------------------//


static void board_serial_port_config(void)
{
	gpio_set_pin_function(BOARD_SERIAL_TX_PORT, BOARD_SERIAL_TX_PIN, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(BOARD_SERIAL_RX_PORT, BOARD_SERIAL_RX_PIN, PERIPHERAL_FUNCTION_D);
}


//--------------------------------------------------------------------------------------------------//


static void board_serial_clock_config(void)
{
	clock_peripheral_clock_enable(ID_USART1);
}


//--------------------------------------------------------------------------------------------------//


static void board_serial_mode_config(void)
{
	usart_mode_config(USART1, USART_STOP_BIT_1, USART_PARITY_NO, USART_MODE_ASYNCHRONOUS, BOARD_SERIAL_CD_VALUE);
	usart_interrupt_enable(USART1, USART_IRQ_RX_READY);
	interrupt_enable_peripheral_interrupt(USART1_IRQn, IRQ_LEVEL_6);
}


//--------------------------------------------------------------------------------------------------//


void board_serial_config(void)
{
	board_serial_port_config();
	board_serial_clock_config();
	board_serial_mode_config();
	
	board_serial_dma_config();
}


//--------------------------------------------------------------------------------------------------//


void board_print_buffer(char* buffer, uint32_t size)
{
	dma_microblock_transaction_descriptor dma;
	
	dma.burst_size = DMA_BURST_SIZE_SINGLE;
	dma.chunk_size = DMA_CHUNK_SIZE_1;
	dma.data_width = DMA_DATA_WIDTH_BYTE;
	
	dma.destination_adressing_mode = DMA_DEST_ADDRESSING_FIXED;
	dma.destination_bus_interface = DMA_AHB_INTERFACE_1;
	dma.destination_pointer = (uint32_t *)(&(USART1->US_THR));
	
	dma.memory_fill = DMA_MEMORY_FILL_OFF;
	dma.peripheral_id = XDMAC_CC_PERID_USART1_TX_Val;
	
	dma.size = size;
	
	dma.channel = 6;
	
	dma.source_addressing_mode = DMA_SOURCE_ADDRESSING_INCREMENTED;
	dma.source_bus_inteface = DMA_AHB_INTERFACE_0;
	dma.source_pointer = (uint32_t *)buffer;
	
	dma.synchronization = DMA_SYNC_MEMORY_TO_PERIPHERAL;
	dma.transfer_type = DMA_TRANSFER_TYPE_PERIPHERAL_TRANSFER;
	dma.trigger = DMA_TRIGGER_HARDWARE;
	
	uint32_t* cache_addr = (uint32_t *)((uint32_t)buffer & ~((uint32_t)32));
	
	// Cache can only be cleaned at 32-bytes alignment
	SCB_CleanDCache_by_Addr(cache_addr, size + 32);
	
	dma_setup_transaction(XDMAC, &dma);
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


void board_serial_print(char* data, ...)
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

	while (*start != '\0')
	{
		usart_write(USART1, *start);
		start++;
	}
	
	//board_serial_dma_print(start);
}


//--------------------------------------------------------------------------------------------------//


void board_serial_write(char data)
{
	usart_write(USART1, data);
}


//--------------------------------------------------------------------------------------------------//


void board_serial_print_address(char* data, uint32_t addr)
{
	board_serial_print(data);
	board_serial_print("0x");
	for (uint8_t i = 0; i < 8; i++)
	{
		uint8_t val = ((addr >> (4 * (7 - i))) & 0b1111);
		
		if (val < 10)
		{
			board_serial_write('0' + val);
		}
		else
		{
			val -= 10;
			board_serial_write('A' + val);
		}
	}
}


//--------------------------------------------------------------------------------------------------//


void board_serial_print_hex(char c)
{
	for (uint8_t i = 0; i < 2; i++)
	{
		uint8_t val = ((c >> (4 * (1 - i))) & 0b1111);
		
		if (val < 10)
		{
			board_serial_write('0' + val);
		}
		else
		{
			val -= 10;
			board_serial_write('A' + val);
		}
	}
}


//--------------------------------------------------------------------------------------------------//


void board_serial_print_percentage_symbol(char* data, uint8_t percent, uint8_t newline)
{
	board_serial_print(data);
	board_serial_print("%d", percent);
	board_serial_write('%');
	if (newline)
	{
		board_serial_write('\n');
	}
}


//--------------------------------------------------------------------------------------------------//


void board_serial_print_register(char* data, uint32_t reg)
{
	board_serial_print("%s: ", data);
	
	for (uint8_t i = 0; i < 32; i++)
	{
		if ((i != 0) && (i % 8 == 0))
		{
			board_serial_write(' ');
		}
		
		if (reg & (1 << (31 - i)))
		{
			board_serial_write('1');
		}
		else
		{
			board_serial_write('0');
		}
	}
	board_serial_write('\n');
}


//--------------------------------------------------------------------------------------------------//


typedef struct
{
	char data[SERIAL_DMA_BUFFER_SIZE];
	uint16_t position;
	
	uint8_t dma_active;
	
} serial_buffer;



static volatile serial_buffer buffer_a;
static volatile serial_buffer buffer_b;


static volatile serial_buffer* current_buffer;
static volatile serial_buffer* dma_buffer;


//--------------------------------------------------------------------------------------------------//


#include "scheduler.h"
#include "list.h"

extern struct scheduler_info scheduler;

// This function gets called when a DMA serial transaction has completed. The dma_buffer pointer
// points to data that is successfully transmitted. 

void board_serial_dma_callback(uint8_t channel)
{
	dma_buffer->dma_active = 0;
	dma_buffer->position = 0;
}


//--------------------------------------------------------------------------------------------------//


// This function will set up a DMA serial transaction 

void board_serial_dma_flush_buffer(char* source_buffer, uint32_t size)
{	
	dma_microblock_transaction_descriptor dma_desc;

	
	// We configure the channel
	dma_desc.burst_size = DMA_BURST_SIZE_SINGLE;
	dma_desc.chunk_size = DMA_CHUNK_SIZE_1;
	dma_desc.data_width = DMA_DATA_WIDTH_BYTE;

	dma_desc.destination_adressing_mode = DMA_DEST_ADDRESSING_FIXED;
	dma_desc.destination_bus_interface = DMA_AHB_INTERFACE_1;
	dma_desc.destination_pointer = (uint32_t *)&(((Usart *)(USART1))->US_THR);

	dma_desc.memory_fill = DMA_MEMORY_FILL_OFF;
	dma_desc.peripheral_id = XDMAC_CC_PERID_USART1_TX_Val;

	dma_desc.size = size;

	dma_desc.channel = BOARD_SERIAL_DMA_CHANNEL;

	dma_desc.source_addressing_mode = DMA_SOURCE_ADDRESSING_INCREMENTED;
	dma_desc.source_bus_inteface = DMA_AHB_INTERFACE_0;
	dma_desc.source_pointer = (uint32_t *)source_buffer;

	dma_desc.synchronization = DMA_SYNC_MEMORY_TO_PERIPHERAL;
	dma_desc.transfer_type = DMA_TRANSFER_TYPE_PERIPHERAL_TRANSFER;
	dma_desc.trigger = DMA_TRIGGER_HARDWARE;
	
	
	
	// Set a callback handler
	dma_channel_set_callback(BOARD_SERIAL_DMA_CHANNEL, board_serial_dma_callback);


	// The serial buffer that is flushed might be located in the cache. Therefore
	// we have to clean the cache lines that corresponds to the serial buffer. That
	// means that the cache data is written back to memory.
	uint32_t* cache_addr = (uint32_t *)((uint32_t)source_buffer & ~((uint32_t)32));

	// Cache can only be cleaned at 32-bytes alignment
	SCB_CleanDCache_by_Addr(cache_addr, size + 32);
	
	SCB_CleanDCache();

	dma_buffer->dma_active = 1;

	// Start the transfer
	dma_setup_transaction(XDMAC, &dma_desc);
}


//--------------------------------------------------------------------------------------------------//


// The DMA serial interface is a double buffered serial interface
// which allows printing to screen with no processor overhead.
//
// The serial buffer will be flushed when enough bytes are ready to be transmitted,
// or a timer overflow has occurred.

void board_serial_dma_config(void)
{
	// Configure the timer interface (timer 0 channel 0) to overflow after 10 ms
	clock_peripheral_clock_enable(ID_TC0_CHANNEL0);
	
	timer_write_protection_disable(TC0);
	timer_capture_mode_config(TC0, TIMER_CHANNEL_0, TIMER_NONE, TIMER_NONE, TIMER_CAPTURE_MODE, 1, 0, 0, TIMER_INCREMENT_RISING_EDGE, TIMER_CLOCK_MCK_DIV_128);
	timer_set_compare_c(TC0, TIMER_CHANNEL_0, 58595);
	timer_interrupt_enable(TC0, TIMER_CHANNEL_0, TIMER_INTERRUPT_C_COMPARE);
	
	interrupt_enable_peripheral_interrupt(TC0_IRQn, IRQ_LEVEL_6);
	
	dma_buffer = &buffer_b;
	current_buffer = &buffer_a;
	
	current_buffer->dma_active = 0;
	dma_buffer->dma_active = 0;
	
	current_buffer->position = 0;
	dma_buffer->position = 0;
}


//--------------------------------------------------------------------------------------------------//


void board_serial_timer_start(void)
{
	timer_clock_enable(TC0, TIMER_CHANNEL_0);
	timer_software_trigger(TC0, TIMER_CHANNEL_0);
}


//--------------------------------------------------------------------------------------------------//


void board_serial_timer_stop(void)
{
	timer_clock_disable(TC0, TIMER_CHANNEL_0);
}


//--------------------------------------------------------------------------------------------------//


void board_serial_dma_switch_buffers(void)
{
	// We must check that the dma buffer is ready
	while (dma_buffer->dma_active)
	{
		
	}
	
	// After the DMA transaction is complete we switch the buffers
	serial_buffer* tmp = (serial_buffer *)dma_buffer;
	
	dma_buffer = current_buffer;
	current_buffer = tmp;
}


//--------------------------------------------------------------------------------------------------//


void board_serial_dma_print(char* data)
{
	while (*data)
	{
		current_buffer->data[current_buffer->position] = *data;
		
		current_buffer->position++;
		data++;
		
		if (current_buffer->position == SERIAL_DMA_BUFFER_SIZE)
		{
			// We have to disable the timer here
			board_serial_timer_stop();
			
			// The buffer is filled up
			board_serial_dma_switch_buffers();
			
			// Start flushing the DMA buffer
			board_serial_dma_flush_buffer((char *)(dma_buffer->data), dma_buffer->position);
		}
	}
	
	/*
	// The buffer is filled up
	board_serial_dma_switch_buffers();
	
	// Start flushing the DMA buffer
	board_serial_dma_flush_buffer(dma_buffer->data, dma_buffer->position);
	*/
	
	board_serial_timer_start();
}


//--------------------------------------------------------------------------------------------------//


void board_serial_dma_print_size(char* data, uint32_t size)
{
	while (size)
	{
		current_buffer->data[current_buffer->position] = *data;
		
		current_buffer->position++;
		data++;
		size--;
		
		if (current_buffer->position == SERIAL_DMA_BUFFER_SIZE)
		{
			// We have to disable the timer here
			board_serial_timer_stop();
			
			// The buffer is filled up
			board_serial_dma_switch_buffers();
			
			// Start flushing the DMA buffer
			board_serial_dma_flush_buffer((char *)(dma_buffer->data), dma_buffer->position);
		}
	}
	
	/*
	// The buffer is filled up
	board_serial_dma_switch_buffers();
	
	// Start flushing the DMA buffer
	board_serial_dma_flush_buffer(dma_buffer->data, dma_buffer->position);
	*/
	
	board_serial_timer_start();
}


//--------------------------------------------------------------------------------------------------//


void TC0_Handler()
{
	timer_read_interrupt_status(TC0, TIMER_CHANNEL_0);
	
	// We have to disable the timer here
	board_serial_timer_stop();
	
	// The buffer is filled up
	board_serial_dma_switch_buffers();
	
	// Start flushing the DMA buffer
	board_serial_dma_flush_buffer((char *)(dma_buffer->data), dma_buffer->position);
}


//--------------------------------------------------------------------------------------------------//
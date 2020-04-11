// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef DMA_H
#define DMA_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef void (*dma_callback)(uint8_t channel);


//--------------------------------------------------------------------------------------------------//


//interrupt source defines
#define DMA_INTERRUPT_END_OF_BLOCK			XDMAC_CIE_BIE_Msk
#define DMA_INTERRUPT_END_OF_LINKED_LIST	XDMAC_CIE_LIE_Msk
#define DMA_INTERRUPT_END_OF_DISABLE		XDMAC_CIE_DIE_Msk
#define DMA_INTERRUPT_END_OF_FLUSH			XDMAC_CIE_FIE_Msk
#define DMA_INTERRUPT_READ_BUS_ERROR		XDMAC_CIE_RBIE_Msk
#define DMA_INTERRUPT_WRITE_BUS_ERROR		XDMAC_CIE_WBIE_Msk
#define DMA_INTERRUPT_REQUEST_OVERFLOW		XDMAC_CIE_ROIE_Msk
#define DMA_INTERRUPT_ALL					0b1111111

#define DMA_NUMBER_OF_CHANNELS	24


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	DMA_DEST_ADDRESSING_FIXED,
	DMA_DEST_ADDRESSING_INCREMENTED,
	DMA_DEST_ADDRESSING_MICROBLOCK_STRIDE,
	DMA_DEST_ADDRESSING_MICROBLOCK_STRIDE_DATA_STRIDE
} dma_destination_addressing_mode_e;



typedef enum
{
	DMA_SOURCE_ADDRESSING_FIXED,
	DMA_SOURCE_ADDRESSING_INCREMENTED,
	DMA_SOURCE_ADDRESSING_MICROBLOCK_STRIDE,
	DMA_SOURCE_ADDRESSING_MICROBLOCK_STRIDE_DATA_STRIDE
} dma_source_addressing_mode_e;



typedef enum
{
	DMA_AHB_INTERFACE_0,
	DMA_AHB_INTERFACE_1
} dma_system_bus_select_e;



typedef enum
{
	DMA_DATA_WIDTH_BYTE,
	DMA_DATA_WIDTH_HALFWORD,
	DMA_DATA_WIDTH_WORD
} dma_data_width_e;



typedef enum
{
	DMA_CHUNK_SIZE_1,
	DMA_CHUNK_SIZE_2,
	DMA_CHUNK_SIZE_4,
	DMA_CHUNK_SIZE_8,
	DMA_CHUNK_SIZE_16
} dma_chunk_size_e;



typedef enum
{
	DMA_MEMORY_FILL_OFF,
	DMA_MEMORY_FILL_ON
} dma_memory_fill_e;



typedef enum
{
	DMA_TRIGGER_HARDWARE,
	DMA_TRIGGER_SOFTWARE
} dma_trigger_e;



typedef enum
{
	DMA_SYNC_PERIPHERAL_TO_MEMORY,
	DMA_SYNC_MEMORY_TO_PERIPHERAL
} dma_synchronization_e;



typedef enum
{
	DMA_BURST_SIZE_SINGLE,
	DMA_BURST_SIZE_FOUR,
	DMA_BURST_SIZE_EIGHT,
	DMA_BURST_SIZE_SIXTEEN
} dma_burst_size_e;



typedef enum
{
	DMA_TRANSFER_TYPE_MEMORY_TRANSFER,
	DMA_TRANSFER_TYPE_PERIPHERAL_TRANSFER
} dma_transfer_type_e;


//--------------------------------------------------------------------------------------------------//


// This is the descriptor that threads uses to instruct the DMA core
typedef struct
{
	uint32_t* source_pointer;
	uint32_t* destination_pointer;
	
	uint32_t size;
	
	// The descriptor will be passed as reference,
	// so that this field can be updated
	int8_t channel;
	
	// Channel config
	uint8_t peripheral_id;
	dma_destination_addressing_mode_e destination_adressing_mode;
	dma_source_addressing_mode_e source_addressing_mode;
	dma_system_bus_select_e destination_bus_interface;
	dma_system_bus_select_e source_bus_inteface;
	dma_data_width_e data_width;
	dma_chunk_size_e chunk_size;
	dma_memory_fill_e memory_fill;
	dma_trigger_e trigger;
	dma_synchronization_e synchronization;
	dma_burst_size_e burst_size;
	dma_transfer_type_e transfer_type;
	
} dma_microblock_transaction_descriptor;


//--------------------------------------------------------------------------------------------------//


void dma_reset(Xdmac* hardware);

uint32_t dma_get_info(Xdmac* hardware);

void dma_config(void);

void dma_clear_unused_register(Xdmac* hardware, uint8_t channel_number);


//--------------------------------------------------------------------------------------------------//


void dma_global_interrupt_enable(Xdmac* hardware, uint8_t channel_number);

void dma_global_interrupt_disable(Xdmac* hardware, uint8_t channel_number);

uint32_t dma_read_global_interrupt_status_register(Xdmac* hardware);

uint32_t dma_read_global_interrupt_mask_register(Xdmac* hardware);


//--------------------------------------------------------------------------------------------------//


void dma_global_suspend_source_request(Xdmac* hardware, uint8_t channel_number);

void dma_global_resume_source_request(Xdmac* hardware, uint8_t channel_number);

void dma_global_suspend_destination_request(Xdmac* hardware, uint8_t channel_number);

void dma_global_resume_destination_request(Xdmac* hardware, uint8_t channel_number);

void dma_global_suspend_rw_request(Xdmac* hardware, uint8_t channel_number);

void dma_global_resume_rw_request(Xdmac* hardware, uint8_t channel_number);


//--------------------------------------------------------------------------------------------------//


void dma_channel_interrupt_enable(Xdmac* hardware, uint8_t channel_number, uint32_t interrupt_mask);

void dma_channel_interrupt_disable(Xdmac* hardware, uint8_t channel_number, uint32_t interrupt_mask);

uint32_t dma_read_channel_interrupt_status_register(Xdmac* hardware, uint8_t channel_number);

uint32_t dma_read_channel_interrupt_mask_register(Xdmac* hardware, uint8_t channel_number);


//--------------------------------------------------------------------------------------------------//


void dma_channel_enable(Xdmac* hardware, uint8_t channel_number);

void dma_channel_disable(Xdmac* hardware, uint8_t channel_number);

uint8_t dma_read_channel_status(Xdmac* hardware, uint8_t channel_number);


//--------------------------------------------------------------------------------------------------//


void dma_channel_mode_config(	Xdmac* hardware,
								uint8_t channel_number,
								uint8_t peripheral_id,
								dma_destination_addressing_mode_e destination_adressing_mode,
								dma_source_addressing_mode_e source_addressing_mode,
								dma_system_bus_select_e destination_bus_interface,
								dma_system_bus_select_e source_bus_inteface,
								dma_data_width_e data_width,
								dma_chunk_size_e chunk_size,
								dma_memory_fill_e memory_fill,
								dma_trigger_e trigger,
								dma_synchronization_e synchronization,
								dma_burst_size_e burst_size,
								dma_transfer_type_e transfer_type);
								
void dma_setup_transaction(Xdmac* hardware, dma_microblock_transaction_descriptor* dma_descriptor);

void dma_channel_set_destination_address(Xdmac* hardware, uint8_t channel_number, const void* const destination_address);

void dma_channel_set_source_address(Xdmac* hardware, uint8_t channel_number, const void* const source_address);

void dma_channel_set_microblock_length(Xdmac* hardware, uint8_t channel_number, uint32_t length);

void dma_channel_set_block_length(Xdmac* hardware, uint8_t channel_number, uint32_t length);

void dma_channel_software_trigger_request(Xdmac* hardware, uint8_t channel_number);

void dma_channel_software_flush(Xdmac* hardware, uint8_t channel_number);

void dma_channel_set_data_stride_memory_pattern(Xdmac* hardware, uint8_t channel_number, int16_t destination_data_stride, int16_t source_data_stride);

void dma_channel_set_microblock_stride_memory_pattern(Xdmac* hardware, uint8_t channel_number, int32_t stride);

void dma_channel_configure(Xdmac* hardware, dma_microblock_transaction_descriptor* dma_descriptor);

void dma_channel_set_callback(uint8_t dma_channel, dma_callback callback);


//--------------------------------------------------------------------------------------------------//


uint32_t dma_read_channel_status_register(Xdmac* hardware);


//--------------------------------------------------------------------------------------------------//


#endif
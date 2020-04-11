// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "dma.h"
#include "core.h"
#include "clock.h"
#include "interrupt.h"
#include "config.h"
#include "check.h"
#include "board_serial.h"


//--------------------------------------------------------------------------------------------------//


static dma_callback dma_handlers[10];


//--------------------------------------------------------------------------------------------------//


void dma_reset(Xdmac* hardware)
{
	// Clear interrupt status bits
	// TODO: This is wrong
	for (uint8_t i = 0; i < DMA_NUMBER_OF_CHANNELS; i++)
	{
		dma_read_channel_interrupt_status_register(hardware, i);
	}
	
	// Clear channel registers that may fuck with my high
	for (uint8_t i = 0; i < DMA_NUMBER_OF_CHANNELS; i++)
	{
		CRITICAL_SECTION_ENTER()
		hardware->XdmacChid[i].XDMAC_CNDC = 0x0;
		hardware->XdmacChid[i].XDMAC_CBC = 0x0;
		hardware->XdmacChid[i].XDMAC_CDS_MSP = 0x0;
		hardware->XdmacChid[i].XDMAC_CSUS = 0x0;
		hardware->XdmacChid[i].XDMAC_CDUS = 0x0;
		CRITICAL_SECTION_LEAVE()
	}
}


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
								dma_transfer_type_e transfer_type)
{
	uint32_t tmp_reg =	(XDMAC_CC_PERID_Msk & (peripheral_id << XDMAC_CC_PERID_Pos)) |
						(destination_adressing_mode << XDMAC_CC_DAM_Pos) |
						(source_addressing_mode << XDMAC_CC_SAM_Pos) |
						(destination_bus_interface << XDMAC_CC_DIF_Pos) |
						(source_bus_inteface << XDMAC_CC_SIF_Pos) |
						(data_width << XDMAC_CC_DWIDTH_Pos) |
						(chunk_size << XDMAC_CC_CSIZE_Pos) |
						(memory_fill << XDMAC_CC_MEMSET_Pos) |
						(trigger << XDMAC_CC_SWREQ_Pos) |
						(synchronization << XDMAC_CC_DSYNC_Pos) |
						(burst_size << XDMAC_CC_MBSIZE_Pos) |
						(transfer_type << XDMAC_CC_TYPE_Pos);
	
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CC = tmp_reg;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_clear_unused_register(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CNDC = 0x0;
	hardware->XdmacChid[channel_number].XDMAC_CBC = 0x0;
	hardware->XdmacChid[channel_number].XDMAC_CDS_MSP = 0x0;
	hardware->XdmacChid[channel_number].XDMAC_CSUS = 0x0;
	hardware->XdmacChid[channel_number].XDMAC_CDUS = 0x0;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t dma_get_info(Xdmac* hardware)
{
	uint32_t tmp = hardware->XDMAC_GTYPE;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void dma_config(void)
{
	clock_peripheral_clock_enable(ID_XDMAC);
	
	// Perform a soft reset
	dma_reset(XDMAC);
	
	// Enable DMA interrupt
	interrupt_enable_peripheral_interrupt(XDMAC_IRQn, DMA_INTERRUPT_PRIORITY);
}


//--------------------------------------------------------------------------------------------------//


void dma_global_interrupt_enable(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GIE = (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_global_interrupt_disable(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GID = (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t dma_read_global_interrupt_status_register(Xdmac* hardware)
{
	uint32_t tmp = hardware->XDMAC_GIS;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t dma_read_global_interrupt_mask_register(Xdmac* hardware)
{
	uint32_t tmp = hardware->XDMAC_GIM;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void dma_global_suspend_source_request(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GRS |= (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_global_resume_source_request(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GRS &= ~(1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_global_suspend_destination_request(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GWS |= (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_global_resume_destination_request(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GWS &= ~(1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_global_suspend_rw_request(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GRWS = (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_global_resume_rw_request(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GRWR = (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_interrupt_enable(Xdmac* hardware, uint8_t channel_number, uint32_t interrupt_mask)
{
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CIE = interrupt_mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_interrupt_disable(Xdmac* hardware, uint8_t channel_number, uint32_t interrupt_mask)
{
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CID = interrupt_mask;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t dma_read_channel_interrupt_status_register(Xdmac* hardware, uint8_t channel_number)
{
	uint32_t tmp = hardware->XdmacChid[channel_number].XDMAC_CIS;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


uint32_t dma_read_channel_interrupt_mask_register(Xdmac* hardware, uint8_t channel_number)
{
	uint32_t tmp = hardware->XdmacChid[channel_number].XDMAC_CIM;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_enable(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GE = (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_disable(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GD = (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint8_t dma_read_channel_status(Xdmac* hardware, uint8_t channel_number)
{
	uint32_t tmp = hardware->XDMAC_GS;
	
	if (tmp & (1 << channel_number))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_set_destination_address(Xdmac* hardware, uint8_t channel_number, const void* const destination_address)
{
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CDA = (uint32_t)destination_address;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_set_source_address(Xdmac* hardware, uint8_t channel_number, const void* const source_address)
{
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CSA = (uint32_t)source_address;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_set_microblock_length(Xdmac* hardware, uint8_t channel_number, uint32_t length)
{
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CUBC = (XDMAC_CUBC_UBLEN_Msk & length);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_set_block_length(Xdmac* hardware, uint8_t channel_number, uint32_t length)
{
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CBC = (XDMAC_CBC_BLEN_Msk & length);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_software_trigger_request(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GSWR = (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_software_flush(Xdmac* hardware, uint8_t channel_number)
{
	CRITICAL_SECTION_ENTER()
	hardware->XDMAC_GSWF = (1 << channel_number);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_set_data_stride_memory_pattern(Xdmac* hardware, uint8_t channel_number, int16_t destination_data_stride, int16_t source_data_stride)
{
	//set data stride pattern
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CDS_MSP = (((XDMAC_CDS_MSP_SDS_MSP_Msk & source_data_stride) << XDMAC_CDS_MSP_SDS_MSP_Pos) |
														((XDMAC_CDS_MSP_DDS_MSP_Msk & destination_data_stride) << XDMAC_CDS_MSP_DDS_MSP_Pos));
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_set_microblock_stride_memory_pattern(Xdmac* hardware, uint8_t channel_number, int32_t stride)
{
	int32_t new_stride = stride & XDMAC_CSUS_SUBS_Msk;
	
	// Calculate two's compliment
	new_stride = ~new_stride;
	new_stride++;
	
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[channel_number].XDMAC_CSUS = new_stride;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t dma_read_channel_status_register(Xdmac* hardware)
{
	uint32_t tmp_reg = hardware->XDMAC_GS;

	return tmp_reg;
}


//--------------------------------------------------------------------------------------------------//


void dma_setup_transaction(Xdmac* hardware, dma_microblock_transaction_descriptor* dma_descriptor)
{	
	// The recipe for performing a single micro block transaction is
	//
	// Read the channel status register to choose a free channel
	// Clear pending status bits
	// Write source address, destination address and size
	// Program the config register
	// Clear 5 registers
	// Enable micro block interrupt
	// Enable channel
	
	uint32_t channel_status = dma_read_channel_status_register(hardware);
	
	int8_t current_channel = -1;
	
	if (dma_descriptor->channel < 0)
	{
		for (uint8_t i = 0; i < DMA_NUMBER_OF_CHANNELS; i++)
		{
			if ((channel_status & (1 << i)) == 0)
			{
				board_serial_print("Channel #%d\n", i);
				current_channel = i;
				break;
			}
		}
	}
	else
	{
		if (channel_status & (1 << dma_descriptor->channel))
		{
			
		}
		else
		{
			current_channel = dma_descriptor->channel;
		}
	}
	
	if (current_channel < 0)
	{
		// Update the descriptor
		// This means that the application should try again
		board_serial_print("No channel available");
		dma_descriptor->channel = -1;
		return;
	}
	
	dma_channel_configure(hardware, dma_descriptor);
	
	// Now we have gotten a free DMA channel, so we can configure the channel
	dma_channel_interrupt_disable(hardware, current_channel, DMA_INTERRUPT_ALL);
	
	// Set addresses and size
	dma_channel_set_source_address(hardware, current_channel, dma_descriptor->source_pointer);
	dma_channel_set_destination_address(hardware, current_channel, dma_descriptor->destination_pointer);
	dma_channel_set_microblock_length(hardware, current_channel, dma_descriptor->size);
	
	// Clear interrupts that can mess with my high
	dma_clear_unused_register(hardware, current_channel);
	
	// Enable interrupts
	dma_global_interrupt_enable(hardware, current_channel);
	dma_channel_interrupt_enable(hardware, current_channel, DMA_INTERRUPT_END_OF_BLOCK);
	
	// Trigger the DMA transaction
	dma_channel_enable(hardware, current_channel);
}


//--------------------------------------------------------------------------------------------------//


void dma_channel_configure(Xdmac* hardware, dma_microblock_transaction_descriptor* dma_descriptor)
{
	uint32_t dma_config_register =	(XDMAC_CC_PERID_Msk & (dma_descriptor->peripheral_id << XDMAC_CC_PERID_Pos)) |
									(dma_descriptor->destination_adressing_mode << XDMAC_CC_DAM_Pos) |
									(dma_descriptor->source_addressing_mode << XDMAC_CC_SAM_Pos) |
									(dma_descriptor->destination_bus_interface << XDMAC_CC_DIF_Pos) |
									(dma_descriptor->source_bus_inteface << XDMAC_CC_SIF_Pos) |
									(dma_descriptor->data_width << XDMAC_CC_DWIDTH_Pos) |
									(dma_descriptor->chunk_size << XDMAC_CC_CSIZE_Pos) |
									(dma_descriptor->memory_fill << XDMAC_CC_MEMSET_Pos) |
									(dma_descriptor->trigger << XDMAC_CC_SWREQ_Pos) |
									(dma_descriptor->synchronization << XDMAC_CC_DSYNC_Pos) |
									(dma_descriptor->burst_size << XDMAC_CC_MBSIZE_Pos) |
									(dma_descriptor->transfer_type << XDMAC_CC_TYPE_Pos);

	check(dma_descriptor->channel >= 0);
	
	CRITICAL_SECTION_ENTER()
	hardware->XdmacChid[dma_descriptor->channel].XDMAC_CC = dma_config_register;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


// This functions set a custom callback for the DMA channels

void dma_channel_set_callback(uint8_t dma_channel, dma_callback callback)
{
	dma_handlers[dma_channel] = callback;
}


//--------------------------------------------------------------------------------------------------//


void XDMAC_Handler()
{
	int8_t source_channel = -1;
	uint32_t global_status = dma_read_global_interrupt_status_register(XDMAC);
	
	// Iterate through all the channels
	for (uint8_t i = 0; i < DMA_NUMBER_OF_CHANNELS; i++)
	{
		if (global_status & (1 << i))
		{
			source_channel = i;
			break;
		}
	}
	
	if (source_channel < 0)
	{
		return;
	}
	
	uint32_t channel_status = dma_read_channel_interrupt_status_register(XDMAC, source_channel);
	
	if (channel_status & (XDMAC_CIS_ROIS_Msk | XDMAC_CIS_WBEIS_Msk | XDMAC_CIS_RBEIS_Msk))
	{
		// An error has occurred
		board_serial_print("DMA Error on channel %d\n", source_channel);
		board_serial_print_register("Status code: ", channel_status);
	}
	else if (channel_status & XDMAC_CIS_BIS_Msk)
	{
		// End of micro block
		// We call the appropriate handler
		dma_handlers[source_channel](source_channel);
	}
}


//--------------------------------------------------------------------------------------------------//
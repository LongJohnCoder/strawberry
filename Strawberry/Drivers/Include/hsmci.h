// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef HSMCI_H
#define HSMCI_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


#define HSMCI_DEBUG			1

#define HSMCI_DMA_CHANNEL	8

#define HSMCI_STATUS_REGISTER_ERROR_MASK ((1 << HSMCI_SR_ACKRCV_Pos) | (1 << HSMCI_SR_BLKOVRE_Pos) | (1 << HSMCI_SR_CSTOE_Pos) | (1 << HSMCI_SR_DTOE_Pos) | (1 << HSMCI_SR_DCRCE_Pos) | (1 << HSMCI_SR_RTOE_Pos) | (1 << HSMCI_SR_CSTOE_Pos) | (1 << HSMCI_SR_RENDE_Pos) | (1 << HSMCI_SR_RDIRE_Pos) | (1 << HSMCI_SR_RINDE_Pos))


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	HSMCI_MR_PADDING_00,
	HSMCI_MR_PADDING_FF
} hsmci_mr_padding_e;



typedef enum
{
	HSMCI_TIMEOUT_MULTIPLIER_1,
	HSMCI_TIMEOUT_MULTIPLIER_16,
	HSMCI_TIMEOUT_MULTIPLIER_128,
	HSMCI_TIMEOUT_MULTIPLIER_256,
	HSMCI_TIMEOUT_MULTIPLIER_1024,
	HSMCI_TIMEOUT_MULTIPLIER_4096,
	HSMCI_TIMEOUT_MULTIPLIER_65536,
	HSMCI_TIMEOUT_MULTIPLIER_1048576
} hsmci_data_timeout_multiplier_e;



typedef enum
{
	HSMCI_SD_BUS_WIDTH_1_BIT = 0,
	HSMCI_SD_BUS_WIDTH_4_BIT = 2,
	HSMCI_SD_BUS_WIDTH_8_BIT = 3
} hsmci_sd_bus_width_e;



typedef enum
{
	HSMCI_SLOT_A,
	HSMCI_SLOT_B,
	HSMCI_SLOT_C,
	HSMCI_SLOT_D
} hsmci_sd_slot_select_e;



typedef enum
{
	HSMCI_COMMAND_SDIO_SPECIAL_COMMAND_STD,
	HSMCI_COMMAND_SDIO_SPECIAL_COMMAND_SUSPEND,
	HSMCI_COMMAND_SDIO_SPECIAL_COMMAND_RESUME
} hsmci_command_sdio_special_command_e;



typedef enum
{
	HSMCI_COMMAND_TRANSFER_TYPE_SINGLE,
	HSMCI_COMMAND_TRANSFER_TYPE_MULTIPLE,
	HSMCI_COMMAND_TRANSFER_TYPE_STREAM,
	HSMCI_COMMAND_TRANSFER_TYPE_BYTE,
	HSMCI_COMMAND_TRANSFER_TYPE_BLOCK
} hsmci_command_transfer_type_e;



typedef enum
{
	HSMCI_COMMAND_TRANSFER_DIRECTION_WRITE,
	HSMCI_COMMAND_TRANSFER_DIRECTION_READ
} hsmci_command_transfer_direction_e;



typedef enum
{
	HSMCI_COMMAND_TRANSFER_COMMAND_NO_DATA,
	HSMCI_COMMAND_TRANSFER_COMMAND_START_DATA,
	HSMCI_COMMAND_TRANSFER_COMMAND_STOP_DATA,
} hsmci_command_transfer_command_e;



typedef enum
{
	HSMCI_COMMAND_MAX_LATENCY_5_CYCLE,
	HSMCI_COMMAND_MAX_LATENCY_64_CYCLE
} hsmci_command_max_latency_e;



typedef enum
{
	HSMCI_COMMAND_SPECIAL_COMMAND_STD,
	HSMCI_COMMAND_SPECIAL_COMMAND_INIT,
	HSMCI_COMMAND_SPECIAL_COMMAND_SYNC,
	HSMCI_COMMAND_SPECIAL_COMMAND_CE_ATA,
	HSMCI_COMMAND_SPECIAL_COMMAND_IT_CMD,
	HSMCI_COMMAND_SPECIAL_COMMAND_IT_RESP,
	HSMCI_COMMAND_SPECIAL_COMMAND_BOR,
	HSMCI_COMMAND_SPECIAL_COMMAND_EBO
} hsmci_command_special_command_e;



typedef enum
{
	HSMCI_COMMAND_RESPONCE_TYPE_NORESP,
	HSMCI_COMMAND_RESPONCE_TYPE_48_BIT,
	HSMCI_COMMAND_RESPONCE_TYPE_136_BIT,
	HSMCI_COMMAND_RESPONCE_TYPE_R1B,
} hsmci_command_responce_type_e;



typedef enum
{
	HSMCI_OK,
	HSMCI_ERROR
} hsmci_status_e;



typedef enum
{
	CHECK_CRC,
	DONT_CHECK_CRC
} hsmci_check_crc_e;



typedef enum
{
	HSMCI_POWERSAVE_ON,
	HSMCI_POWERSAVE_OFF
} hsmci_control_powersave_e;



typedef enum
{
	HSMCI_ENABLE,
	HSMCI_DISABLE
} hsmci_control_enable_e;



typedef enum
{
	HSMCI_DMA_CHUNK_1,
	HSMCI_DMA_CHUNK_2,
	HSMCI_DMA_CHUNK_4,
	HSMCI_DMA_CHUNK_8,
	HSMCI_DMA_CHUNK_16
} hsmci_dma_chunk_size_e;

//--------------------------------------------------------------------------------------------------//


void hsmci_write_protection_enable(Hsmci* hardware);

void hsmci_write_protection_disable(Hsmci* hardware);


//--------------------------------------------------------------------------------------------------//


void hsmci_software_reset(Hsmci* hardware);

void hsmci_soft_reset(Hsmci* hardware);

void hsmci_powersave_enable(Hsmci* hardware);

void hsmci_powersave_disable(Hsmci* hardware);

void hsmci_enable(Hsmci* hardware);

void hsmci_disable(Hsmci* hardware);


//--------------------------------------------------------------------------------------------------//


void hsmci_set_completion_timeout(Hsmci* hardware, hsmci_data_timeout_multiplier_e completion_signal_timout_multiplier, uint8_t completion_signal_timeout_cycle_number);
						
void hsmci_set_data_timeout(Hsmci* hardware, hsmci_data_timeout_multiplier_e data_timeout_multiplier, uint8_t data_timeout_cycle_number);


//--------------------------------------------------------------------------------------------------//


void hsmci_write_control_register(Hsmci* hardware, hsmci_control_enable_e enable, hsmci_control_powersave_e powersave);

void hsmci_set_bus_width(Hsmci* hardware, hsmci_sd_bus_width_e bus_width, hsmci_sd_slot_select_e slot_selct);

void hsmci_set_block_length(Hsmci* hardware, uint32_t block_length, uint32_t block_count);


//--------------------------------------------------------------------------------------------------//


void hsmci_write_argument_register(Hsmci* hardware, uint32_t argument);

void hsmci_write_data_register(Hsmci* hardware, uint32_t data);


//--------------------------------------------------------------------------------------------------//


uint32_t hsmci_read_data_register(Hsmci* hardware);

hsmci_status_e hsmci_start_read_blocks(void* destination, uint16_t number_of_blocks);

hsmci_status_e hsmci_wait_end_of_read(void);

void hsmci_read_data_register_reverse(Hsmci* hardware, uint8_t* data, uint8_t number_of_words);

uint32_t hsmci_read_48_bit_response_register(Hsmci* hardware);	

void hsmci_read_136_bit_response_register_extended(Hsmci* hardware, uint8_t* response);
						
uint32_t hsmci_read_status_register(Hsmci* hardware);
			
					
//--------------------------------------------------------------------------------------------------//


hsmci_status_e hsmci_send_command(Hsmci* hardware, uint32_t command_register, uint32_t argument, hsmci_check_crc_e crc);

void hsmci_write_command_register(Hsmci* hardware, uint32_t command);

hsmci_status_e hsmci_send_addressed_data_transfer_command(	Hsmci* hardware, uint32_t command_register, uint32_t argument,
															uint16_t block_size, uint16_t number_of_blocks, uint8_t dma, hsmci_check_crc_e crc);

hsmci_status_e hsmci_stop_addressed_transfer_command(Hsmci* hardware, uint32_t command, uint32_t argument);

uint32_t hsmci_construct_command_register(	uint8_t boot_ack,
										uint8_t ata_with_command_completion_enable,
										hsmci_command_sdio_special_command_e sdio_special_command,
										hsmci_command_transfer_type_e transfer_type,
										hsmci_command_transfer_direction_e transfer_direction,
										hsmci_command_transfer_command_e transfer_command,
										hsmci_command_max_latency_e max_latency,
										uint8_t open_drain_enable,
										hsmci_command_special_command_e special_command,
										hsmci_command_responce_type_e responce_type,
										uint8_t command_number);


//--------------------------------------------------------------------------------------------------//


void hsmci_force_byte_transfer_enable(Hsmci* hardware);

void hsmci_force_byte_transfer_disable(Hsmci* hardware);

void hsmci_read_proof_enable(Hsmci* hardware);

void hsmci_read_proof_disable(Hsmci* hardware);

void hsmci_write_proof_enable(Hsmci* hardware);

void hsmci_write_proof_disable(Hsmci* hardware);

void hsmci_set_padding_value(Hsmci* hardware, hsmci_mr_padding_e padding);

void hsmci_set_bus_speed(Hsmci* hardware, uint32_t bus_speed, uint32_t cpu_peripheral_speed);

void hsmci_write_mode_register(	Hsmci* hardware,
								uint8_t odd_clock_divider,
								hsmci_mr_padding_e padding,
								uint8_t force_byte_tranfer_enable,
								uint8_t write_proof_enable,
								uint8_t read_proof_enable,
								uint8_t power_save_divider,
								uint8_t clock_divider);


//--------------------------------------------------------------------------------------------------//


void hsmci_high_speed_enable(Hsmci* hardware);

void hsmci_dma_enable(Hsmci* hardware, hsmci_dma_chunk_size_e chunk_size);

void hsmci_dma_disable(Hsmci* hardware);

void hsmci_write_configuration_register(Hsmci* hardware,
										uint8_t synhronize_last_block,
										uint8_t high_speed_mode,
										uint8_t flow_error_reset_control_mode,
										uint8_t transfer_after_data_written_to_fifo);


//--------------------------------------------------------------------------------------------------//


#endif
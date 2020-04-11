// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef SPI_H
#define SPI_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	SPI_FIXED_CHIP_SELECT_0 = 0b0000,
	SPI_FIXED_CHIP_SELECT_1 = 0b0001,
	SPI_FIXED_CHIP_SELECT_2 = 0b0011,
	SPI_FIXED_CHIP_SELECT_3 = 0b0111,
	SPI_FIXED_CHIP_SELECT_NONE = 0b1111
} spi_fixed_chip_select_e;



typedef enum
{
	SPI_MODE_SLAVE,
	SPI_MODE_MASTER
} spi_master_slave_mode_e;



typedef enum
{
	SPI_FIXED_PERIPHERAL,
	SPI_VARIABLE_PERIPHERAL	
} spi_peripheral_mode_e;



typedef enum
{	
	SPI_CHIP_SELECT_0,
	SPI_CHIP_SELECT_1,
	SPI_CHIP_SELECT_2,
	SPI_CHIP_SELECT_3
} spi_chip_select_e;



typedef enum
{
	SPI_8_BIT,
	SPI_9_BIT,
	SPI_10_BIT,
	SPI_11_BIT,
	SPI_12_BIT,
	SPI_13_BIT,
	SPI_14_BIT,
	SPI_15_BIT,
	SPI_16_BIT
} spi_bits_per_transfer_e;



typedef enum
{
	SPI_CLOCK_CAPTURE_ON_TRAILING,
	SPI_CLOCK_CAPTURE_ON_LEADING
} spi_clock_phase_e;



typedef enum
{
	SPI_CLOCK_INACTIVE_LOW,
	SPI_CLOCK_INACTIVE_HIGH
}spi_clock_polarity_e;


//--------------------------------------------------------------------------------------------------//


void spi_write_protection_enable(Spi* hardware);

void spi_write_protection_disable(Spi* hardware);


//--------------------------------------------------------------------------------------------------//


// This functions only configures general parameters of the SPI module

void spi_mode_config(Spi* hardware, uint8_t delay_between_chip_selekt, spi_fixed_chip_select_e fixed_chip_select, 
						uint8_t local_loopback_enable, uint8_t wait_data_read_befor_transfere,
						uint8_t mode_fault_detection_disable, uint8_t chip_select_decode_enable,
						spi_peripheral_mode_e peripheral_mode , spi_master_slave_mode_e master_slave_mode);


// This functions configures each chip select with parameter such as
//		- Transfer delays
//		- Clock phase
//		- Clock polarity
//		- Bus speed

void spi_chip_select_config(Spi* hardware, spi_chip_select_e chip_select, uint8_t delay_between_consecutive_transfers,
							uint8_t delay_before_clock, uint8_t serial_clock_bit_rate, spi_bits_per_transfer_e bits_per_transfer,
							uint8_t chip_select_active_after_transfer, uint8_t chip_select_not_active_after_transfer,
							spi_clock_polarity_e clock_polarity, spi_clock_phase_e clock_phase);


void spi_set_bit_size(Spi* hardware, spi_chip_select_e chip_select, spi_bits_per_transfer_e bits_per_transfer);

void spi_set_bus_speed(Spi* hardware, spi_chip_select_e chip_slect, uint8_t bit_rate);


//--------------------------------------------------------------------------------------------------//


void spi_enable(Spi* hardware);

void spi_disable(Spi* hardware);

void spi_software_reset (Spi* hardware);


//--------------------------------------------------------------------------------------------------//


void spi_trasmit_data_16_bit(Spi* hardware, uint16_t transmit_data);

void spi_trasmit_data_8_bit(Spi* hardware, uint8_t transmit_data);

void spi_flush(Spi* hardware);


//--------------------------------------------------------------------------------------------------//


uint32_t spi_get_status_register(Spi* hardware);


//--------------------------------------------------------------------------------------------------//


#endif
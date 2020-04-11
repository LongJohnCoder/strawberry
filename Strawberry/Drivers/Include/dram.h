// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef DRAM_H
#define DRAM_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	DRAM_MODE_NORMAL = 0,
	DRAM_MODE_MODE_NOP,
	DRAM_MODE_ALL_BANKS_PRECHARGE,
	DRAM_MODE_LOAD_MODE_REGISTER,
	DRAM_MODE_AUTO_REFRESH,
	DRAM_MODE_EXT_LOAD_MODE_REGISTER,
	DRAM_MODE_DEEP_POWERDOWN
} sram_mode_e;



typedef enum
{
	DRAM_CAS_LATENCY_1 = 1,
	DRAM_CAS_LATENCY_2,
	DRAM_CAS_LATENCY_3
} sram_cas_latency_e;



typedef enum
{
	DRAM_BANK_2 = 0,
	DRAM_BANK_4
} sram_number_of_banks_e;



typedef enum
{
	DRAM_ROW_BITS_11 = 0,
	DRAM_ROW_BITS_12,
	DRAM_ROW_BITS_13
} sram_number_of_row_bits_e;



typedef enum
{
	DRAM_COL_BITS_8 = 0,
	DRAM_COL_BITS_9,
	DRAM_COL_BITS_10,
	DRAM_COL_BITS_11
} sram_number_of_col_bits_e;



typedef enum
{
	DRAM_TIMEOUT_LP_LAST_XFER = 0,
	DRAM_TIMEOUT_LP_LAST_XFER_64,
	DRAM_TIMEOUT_LP_LAST_XFER_128
} sram_timeout_e;



typedef enum
{
	DRAM_LOW_POWER_DISABLED = 0,
	DRAM_LOW_POWER_SELF_REFRESH,
	DRAM_LOW_POWER_POWER_DOWN,
	DRAM_LOW_POWER_DEEP_POWER_DOWN
} dram_low_power_configuration_e;



typedef enum
{
	DRAM_MEMORY_DEVICE_SDRAM,
	DRAM_MEMORY_DEVICE_LPSDRAM
} dram_memory_device_e;



typedef enum
{
	DRAM_UNALIGNED_ACCESS_UNSUPPORTED,
	DRAM_UNALIGNED_ACCESS_SUPPORTED
} dram_unaligned_access_e;


//--------------------------------------------------------------------------------------------------//


void dram_config(void);

void dram_write_mode_register(Sdramc* hardware, sram_mode_e ram_mode);

void dram_set_refresh_timer(Sdramc* hardware, uint16_t value);

void dram_write_configuration_register(	Sdramc* hardware,
										uint8_t exit_self_refresh_to_active_delay,
										uint8_t active_to_precharge_delay,
										uint8_t row_to_column_delay,
										uint8_t row_precharge_delay,
										uint8_t row_cycle_delay,
										uint8_t write_recovery_delay,
										sram_cas_latency_e cas_latency,
										sram_number_of_banks_e number_of_banks,
										sram_number_of_row_bits_e row_bits,
										sram_number_of_col_bits_e col_bits);

void dram_write_configuration_register_1(	Sdramc* hardware,
											dram_unaligned_access_e support_unaligned,
											uint8_t load_mode_to_refresh_delay);

void dram_write_low_power_register(	Sdramc* hardware,
									sram_timeout_e timeout_before_low_power,
									dram_low_power_configuration_e configuration,
									uint8_t drive_strength,
									uint8_t temp_comp_self_refresh,
									uint8_t partial_array_self_refresh);

void dram_interrupt_enable(Sdramc* hardware);

void dram_interrupt_disble(Sdramc* hardware);

uint32_t dram_read_interrupt_status_register(Sdramc* hardware);

void dram_set_memory_device(Sdramc* hardware, dram_memory_device_e device);

void dram_scrambling_enable(Sdramc* hardware);

void dram_scrambling_disable(Sdramc* hardware);

void dram_set_scrambling_keys(Sdramc* hardware, uint32_t key1, uint32_t key2);


//--------------------------------------------------------------------------------------------------//


#endif
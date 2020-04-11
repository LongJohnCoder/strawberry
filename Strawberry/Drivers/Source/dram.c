// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "dram.h"
#include "matrix.h"
#include "core.h"
#include "gpio.h"
#include "clock.h"


//--------------------------------------------------------------------------------------------------//


static void sdram_clock_config(void)
{
	clock_peripheral_clock_enable(ID_SDRAMC);
}


//--------------------------------------------------------------------------------------------------//


static void sdram_pin_config(void)
{
	gpio_set_pin_function(PIOC, 20, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 21, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 22, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 23, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 24, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 25, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 26, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 27, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 28, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 29, PERIPHERAL_FUNCTION_A);
	
	gpio_set_pin_function(PIOA, 20, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOD, 17, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOC, 0, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 1, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 2, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 3, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 4, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 5, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 6, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 7, PERIPHERAL_FUNCTION_A);
	
	gpio_set_pin_function(PIOE, 0, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOE, 1, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOE, 2, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOE, 3, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOE, 4, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOE, 5, PERIPHERAL_FUNCTION_A);
	
	gpio_set_pin_function(PIOA, 15, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOA, 16, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOC, 18, PERIPHERAL_FUNCTION_A);
	
	gpio_set_pin_function(PIOD, 15, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOD, 16, PERIPHERAL_FUNCTION_C);
	
	gpio_set_pin_function(PIOD, 13, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOD, 23, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOD, 14, PERIPHERAL_FUNCTION_C);
	gpio_set_pin_function(PIOC, 15, PERIPHERAL_FUNCTION_A);
	gpio_set_pin_function(PIOD, 29, PERIPHERAL_FUNCTION_C);
}


//--------------------------------------------------------------------------------------------------//


void dram_config(void)
{
	sdram_clock_config();
	sdram_pin_config();
	
	//get the start address of the SDRAM
	volatile uint16_t* dram_base_address = (uint16_t *)SDRAM_CS_ADDR;
	volatile uint32_t i;
	
	//start of initialization process. Refer to data sheet
	//set chip select 1 to SDRAM in the bus MATRIX
	matrix_write_protection_disable(MATRIX);
	matrix_write_flash_chip_select_configuration_register(MATRIX, MATRIX_CS_SDRAM, 0, 0, 0);
	
	//step 1
	//write CR and CFR1 register
	dram_write_configuration_register(	SDRAMC,
	15, 9, 5, 5, 13, 5,
	DRAM_CAS_LATENCY_3, DRAM_BANK_2,
	DRAM_ROW_BITS_11, DRAM_COL_BITS_8);

	
	dram_write_configuration_register_1(SDRAMC, DRAM_UNALIGNED_ACCESS_SUPPORTED, 2);
	
	//step 2
	//configure low power register
	SDRAMC->SDRAMC_LPR = 0x00;
	
	//step 3
	//select SDRAM memory type
	dram_set_memory_device(SDRAMC, DRAM_MEMORY_DEVICE_SDRAM);
	
	//step 4
	//pause of 200 us
	for (i = 0; i < ((150000000 / 1000000) * 200 / 6); i++) {
		;
	}
	
	//step 5
	//issue a NOP command and perform any write operation
	dram_write_mode_register(SDRAMC, DRAM_MODE_MODE_NOP);
	*dram_base_address = 0x00;
	
	// Step 6
	// Charge all banks
	dram_write_mode_register(SDRAMC, DRAM_MODE_ALL_BANKS_PRECHARGE);
	*dram_base_address = 0x00;
	for (i = 0; i < ((150000000 / 1000000) * 200 / 6); i++) {
		;
	}
	
	//step 7
	//perform eight auto refresh
	dram_write_mode_register(SDRAMC, DRAM_MODE_AUTO_REFRESH);
	*dram_base_address = 0x01;
	dram_write_mode_register(SDRAMC, DRAM_MODE_AUTO_REFRESH);
	*dram_base_address = 0x02;
	dram_write_mode_register(SDRAMC, DRAM_MODE_AUTO_REFRESH);
	*dram_base_address = 0x03;
	dram_write_mode_register(SDRAMC, DRAM_MODE_AUTO_REFRESH);
	*dram_base_address = 0x04;
	dram_write_mode_register(SDRAMC, DRAM_MODE_AUTO_REFRESH);
	*dram_base_address = 0x05;
	dram_write_mode_register(SDRAMC, DRAM_MODE_AUTO_REFRESH);
	*dram_base_address = 0x06;
	dram_write_mode_register(SDRAMC, DRAM_MODE_AUTO_REFRESH);
	*dram_base_address = 0x07;
	dram_write_mode_register(SDRAMC, DRAM_MODE_AUTO_REFRESH);
	*dram_base_address = 0x08;
	
	//step 8
	dram_write_mode_register(SDRAMC, DRAM_MODE_LOAD_MODE_REGISTER);
	*((uint16_t *)(dram_base_address + 0x30)) = 0xfefa;
	for (i = 0; i < ((150000000 / 1000000) * 200 / 6); i++) {
		;
	}

	
	//step 10
	//go into normal mode
	dram_write_mode_register(SDRAMC, DRAM_MODE_NORMAL);
	*dram_base_address = 0x00;
	
	//step 11
	i = 150000000 / 1000u;
	i *= 15625u;
	i /= 1000000u;
	SDRAMC->SDRAMC_TR = SDRAMC_TR_COUNT(i);
}


//--------------------------------------------------------------------------------------------------//


void dram_write_mode_register(Sdramc* hardware, sram_mode_e ram_mode)
{
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_MR = ram_mode;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dram_set_refresh_timer(Sdramc* hardware, uint16_t value)
{
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_TR = (SDRAMC_TR_COUNT_Msk & value);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dram_write_configuration_register(Sdramc* hardware,
										uint8_t exit_self_refresh_to_active_delay,
										uint8_t active_to_precharge_delay,
										uint8_t row_to_column_delay,
										uint8_t row_precharge_delay,
										uint8_t row_cycle_delay,
										uint8_t write_recovery_delay,
										sram_cas_latency_e cas_latency,
										sram_number_of_banks_e number_of_banks,
										sram_number_of_row_bits_e row_bits,
										sram_number_of_col_bits_e col_bits)
{
	uint32_t tmp_reg =	(SDRAMC_CR_TXSR_Msk & (exit_self_refresh_to_active_delay << SDRAMC_CR_TXSR_Pos)) |
						(SDRAMC_CR_TRAS_Msk & (active_to_precharge_delay << SDRAMC_CR_TRAS_Pos)) |
						(SDRAMC_CR_TRCD_Msk & (row_to_column_delay << SDRAMC_CR_TRCD_Pos)) |
						(SDRAMC_CR_TRP_Msk & (row_precharge_delay << SDRAMC_CR_TRP_Pos)) |
						(SDRAMC_CR_TRC_TRFC_Msk & (row_cycle_delay << SDRAMC_CR_TRC_TRFC_Pos)) |
						(SDRAMC_CR_TWR_Msk & (write_recovery_delay << SDRAMC_CR_TWR_Pos)) |
						(cas_latency << SDRAMC_CR_CAS_Pos) |
						(number_of_banks << SDRAMC_CR_NB_Pos) |
						(row_bits << SDRAMC_CR_NR_Pos) |
						(col_bits << SDRAMC_CR_NC_Pos) |
						(1 << SDRAMC_CR_DBW_Pos);
	
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_CR = tmp_reg;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dram_write_configuration_register_1(	Sdramc* hardware,
											dram_unaligned_access_e support_unaligned,
											uint8_t load_mode_to_refresh_delay)
{
	uint32_t tmp_reg =	(support_unaligned << SDRAMC_CFR1_UNAL_Pos) |
						(SDRAMC_CFR1_TMRD_Msk & (load_mode_to_refresh_delay << SDRAMC_CFR1_TMRD_Pos));
	
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_CFR1 = tmp_reg;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dram_write_low_power_register(	Sdramc* hardware,
									sram_timeout_e timeout_before_low_power,
									dram_low_power_configuration_e configuration,
									uint8_t drive_strength,
									uint8_t temp_comp_self_refresh,
									uint8_t partial_array_self_refresh)
{
	uint32_t tmp_reg =	(timeout_before_low_power << SDRAMC_LPR_TIMEOUT_Pos) |
						(configuration << SDRAMC_LPR_LPCB_Pos) |
						(SDRAMC_LPR_DS_Msk & (drive_strength << SDRAMC_LPR_DS_Pos)) |
						(SDRAMC_LPR_TCSR_Msk & (temp_comp_self_refresh << SDRAMC_LPR_TCSR_Pos)) |
						(SDRAMC_LPR_PASR_Msk & (partial_array_self_refresh << SDRAMC_LPR_PASR_Pos));
	
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_LPR = tmp_reg;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dram_interrupt_enable(Sdramc* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_IER = (1 << SDRAMC_IER_RES_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dram_interrupt_disble(Sdramc* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_IDR = (1 << SDRAMC_IER_RES_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


uint32_t dram_read_interrupt_status_register(Sdramc* hardware)
{
	uint32_t tmp = hardware->SDRAMC_ISR;
	
	return tmp;
}


//--------------------------------------------------------------------------------------------------//


void dram_set_memory_device(Sdramc* hardware, dram_memory_device_e device)
{
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_MDR = device;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dram_scrambling_enable(Sdramc* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_OCMS = 0b1;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dram_scrambling_disable(Sdramc* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_OCMS = 0b0;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void dram_set_scrambling_keys(Sdramc* hardware, uint32_t key1, uint32_t key2)
{
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_OCMS_KEY1 = key1;
	CRITICAL_SECTION_LEAVE()
	
	CRITICAL_SECTION_ENTER()
	hardware->SDRAMC_OCMS_KEY2 = key2;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//
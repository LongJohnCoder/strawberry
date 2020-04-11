// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "matrix.h"
#include "core.h"


//--------------------------------------------------------------------------------------------------//


void matrix_write_protection_disable(Matrix* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->MATRIX_WPMR = (MATRIX_WPMR_WPKEY_PASSWD_Val << MATRIX_WPMR_WPKEY_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void matrix_write_protection_enable(Matrix* hardware)
{
	CRITICAL_SECTION_ENTER()
	hardware->MATRIX_WPMR = (1 << MATRIX_WPMR_WPEN_Pos) | (MATRIX_WPMR_WPKEY_PASSWD_Val << MATRIX_WPMR_WPKEY_Pos);
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//


void matrix_write_flash_chip_select_configuration_register(	Matrix* hardware,
															matrix_chip_select_1_e assign_cs1,
															uint8_t assign_cs2_flash,
															uint8_t assign_cs3_flash,
															uint8_t assign_cs0_flash)
{
	uint32_t tmp_reg =	((0b1 & assign_cs0_flash) << CCFG_SMCNFCS_SMC_NFCS0_Pos) |
	((0b1 & assign_cs2_flash) << CCFG_SMCNFCS_SMC_NFCS2_Pos) |
	((0b1 & assign_cs3_flash) << CCFG_SMCNFCS_SMC_NFCS3_Pos);
	
	if (assign_cs1 == MATRIX_CS_SDRAM)
	{
		tmp_reg |= (1 << CCFG_SMCNFCS_SDRAMEN_Pos);
	}
	else if (assign_cs1 == MATRIX_CS_NAND_FLASH)
	{
		tmp_reg |= (1 << CCFG_SMCNFCS_SMC_NFCS1_Pos);
	}
	
	CRITICAL_SECTION_ENTER()
	hardware->CCFG_SMCNFCS = tmp_reg;
	CRITICAL_SECTION_LEAVE()
}


//--------------------------------------------------------------------------------------------------//
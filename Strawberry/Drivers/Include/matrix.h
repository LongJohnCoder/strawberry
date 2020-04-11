// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef MATRIX_H
#define MATRIX_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	MATRIX_CS_SDRAM,
	MATRIX_CS_NAND_FLASH,
	MATRIX_NONE
} matrix_chip_select_1_e;


//--------------------------------------------------------------------------------------------------//


void matrix_write_protection_disable(Matrix* hardware);

void matrix_write_protection_enable(Matrix* hardware);

void matrix_write_flash_chip_select_configuration_register(	Matrix* hardware,
															matrix_chip_select_1_e assign_cs1,
															uint8_t assign_cs2_flash,
															uint8_t assign_cs3_flash,
															uint8_t assign_cs0_flash);


//--------------------------------------------------------------------------------------------------//


#endif
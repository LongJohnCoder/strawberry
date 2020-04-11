// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef BOARD_SD_CARD_H
#define BOARD_SD_CARD_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef enum
{
	SD_DISCONNECTED,
	SD_CONNECTED
} board_sd_status;


//--------------------------------------------------------------------------------------------------//


void board_sd_card_config(void);

board_sd_status board_sd_card_get_status(void);


//--------------------------------------------------------------------------------------------------//


#endif
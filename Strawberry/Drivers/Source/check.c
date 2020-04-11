// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "check.h"
#include "board_serial.h"


//--------------------------------------------------------------------------------------------------//


#if CHECK_ENABLE_PRINT_HANDLER
void (* check_print_handler)(char* , ...) = CHECK_PRINT_HANDLER;
#endif


//--------------------------------------------------------------------------------------------------//


void check_handler(uint8_t condition, const char* filename, uint32_t line_number)
{
	if (!condition)
	{
		// Print debug information
		#if CHECK_ENABLE_PRINT_HANDLER
		check_print_handler("Check failure\nFile:\t%s\nLine:\t%d\n", filename, line_number);
		#endif
		
		// Trigger a breakpoint		
		#if CHECK_ENABLE_BREAKPOINT
		asm volatile("BKPT #0");
		#endif
	}
}


//--------------------------------------------------------------------------------------------------//
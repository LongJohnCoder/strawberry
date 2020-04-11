// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

// Tell the assembler to enterpret instructions as Thumb (16-bit)
.thumb

// Two different syntaxes are supported for ARM and Thumb
// divided let ARM and Thumb use their own syntax, while
// unified let us use whatever syntax we want
.syntax unified

// Now we have to set the section that the code will be
// placed in. We will use the default section .text and 
// therefore noe .section will be needed
.text


//--------------------------------------------------------------------------------------------------//


// Extern variables are lables that is not found in the current
// source (.s) file. It must be delared as global elsewhere


//--------------------------------------------------------------------------------------------------//


// Declare global functions since these are declared in separate files
.global kernel_service_handler
.global SVCall_Handler

// This handler calls the kernel_service_handler where the
// appropriate function is called
.thumb_func

SVCall_Handler:

	tst				lr,							#4
	ite				eq
	mrseq			r0,							msp
	mrsne			r0,							psp
	b				kernel_service_handler
	bx				lr


//--------------------------------------------------------------------------------------------------//

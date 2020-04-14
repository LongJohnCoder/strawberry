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

// Preserve 8 byte alignment
.balign 16

// Now we have to set the section that the code will be
// placed in. We will use the default section .text and 
// therefore noe .section will be needed
.text

// Extern variables are lables that is not found in the current
// source (.s) file. It must be delared as global elsewhere


//--------------------------------------------------------------------------------------------------//


// Make the current thread pointer visible by declaring it extern
.extern current_thread
.extern next_thread


//--------------------------------------------------------------------------------------------------//


// Declare global functions since these are declared in separate files
.global kernel_scheduler
.global PendSV_Handler

// The SysTick interrupt has already picked the next thread to run. This thread is
// the next_thread. This handler will perform the context switch, that is; store the
// current context in the current_thread and restore the new context from next_thread.
.thumb_func

PendSV_Handler:
	
	// Disable interrupts
	cpsid				i

	// Load the process stack pointer into r0
	mrs					r0,					psp

	// Flush the instruction pipeline
	// This is specified in the Cortex-M7 datasheet
	isb

	// Push register r4-r11 onto the process stack
	// using the store multiple decrement before instruction
	stmdb				r0!,				{r4-r11}

	// Get the pointer to the current thread r1 will hold a pointer
	// to the current thread structure
	ldr					r3,					=current_thread
	ldr					r1,					[r3]

	// Store the stack pointer for the current running thread
	// since this might have changed
	str					r0,					[r1]

	// Load the next thread into r1 and the next stack into r0
	ldr					r2,					=next_thread
	ldr					r1,					[r2]
	ldr					r0,					[r1]

	// Since we are operating with two thread pointers we must make
	// sure that the current pointer gets replaced by the next running thread
	str					r1,					[r3]

	// Pop the registers from stack
	// the stack frame will be restored automatically by the IRQ
	ldmia				r0!,				{r4-r11}

	// Move the R0 (current stack pointer) to the process stack pointer
	msr					psp,				r0

	// Flush the instruction pipeline
	isb

	// Reenable interrupt after context switch
	cpsie				i

	bx					lr


//--------------------------------------------------------------------------------------------------//


// Declare the global function
.global scheduler_start
	
// This function will start the scheduler and begin executing the first thread
.thumb_func

scheduler_start:
	
	// The first line loads a pointer to kernel current thread into r0
	ldr					r0,					=current_thread
	ldr					r2,					[r0]
	ldr					r3,					[r2]
	
	// Load the stack pointer into the PSP
	msr					psp,				r3
	isb

	// We are in thread mode so we can now switch the CPU stack
	movs				r0,					#2
	msr					control,			r0
	isb

	// Pop the registers from the first thread stack
	pop					{r4-r11}
	pop					{r0-r3}
	pop					{r12}

	// We dont want the link register
	add					sp,					sp, #4

	// We pop the address of the first thread to run
	// into the link register
	pop					{lr}

	// We dont want the xPSR register
	add					sp,					sp, #4

	// Reenable interrupt
	cpsie				i
	cpsie				f

	dsb
	isb

	bx					lr


//--------------------------------------------------------------------------------------------------//

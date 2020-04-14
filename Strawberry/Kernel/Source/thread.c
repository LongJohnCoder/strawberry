// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "thread.h"
#include "dynamic_memory.h"
#include "critical_section.h"
#include "check.h"
#include "interrupt.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


extern struct thread_structure* current_thread;
extern struct thread_structure* idle_thread;
extern list_s					running_queue;
extern list_s					thread_list;


//--------------------------------------------------------------------------------------------------//


uint32_t* thread_stack_init(uint32_t* stack_pointer, thread_function thread_func, void* param);

static void round_robin_idle_thread(void* param);

static void kernel_delete_thread(void);


//--------------------------------------------------------------------------------------------------//


void thread_config(void)
{
	// Interrupt should already be enabled
	interrupt_global_disable();
	
	// Add the idle thread on priority level 7 (lowest)
	thread_new("Idle", round_robin_idle_thread, NULL, THREAD_PRIORITY_NORMAL, KERNEL_IDLE_THREAD_STACK_SIZE);
}


//--------------------------------------------------------------------------------------------------//


struct thread_structure* thread_new(char* thread_name, thread_function thread_func, void* thread_parameter, enum thread_priority priority, uint32_t stack_size)
{
	// We do NOT want any scheduler interrupting inside here
	suspend_scheduler();
	
	// First we have to allocate memory for the thread and
	// for the stack that is going to be used by that thread
	struct thread_structure* new_thread = (struct thread_structure*)dynamic_memory_new(DRAM_BANK_0, sizeof(struct thread_structure) + stack_size * sizeof(uint32_t));
	
	// Allocate the stack
	new_thread->stack_base = (uint32_t *)((uint8_t *)new_thread + sizeof(struct thread_structure));
	

	if ((new_thread == NULL) || (new_thread->stack_base == NULL))
	{
		// Allocation failed
		check(0);
		return NULL;
	}
	
	// Get the stack pointer to point to top of stack
	new_thread->stack_pointer = new_thread->stack_base + stack_size - 1;
	
	// Initialize the stack
	new_thread->stack_pointer = thread_stack_init(new_thread->stack_pointer, thread_func, thread_parameter);
	
	// Set the thread name
	for (uint32_t i = 0; i < KERNEL_THREAD_MAX_NAME_LENGTH; i++)
	{
		// We have reached the end of the string
		if (*thread_name == '\0')
		{
			break;
		}
		new_thread->name[i] = *thread_name++;
	}
	
	// Set the thread priority
	new_thread->priority = priority;
	new_thread->state = THREAD_STATE_RUNNING;
	new_thread->stack_size = 4 * stack_size;
	
	
	// The first thread to be made is the IDLE thread
	if (idle_thread == NULL)
	{
		// Just loop that single thread
		new_thread->next = new_thread;
		
		idle_thread = new_thread;
	}
	else
	{
		new_thread->current_list = &running_queue;
		new_thread->next_list = NULL;
		new_thread->list_node.object = new_thread;
		new_thread->thread_list.object = new_thread;
		
		list_insert_first(&(new_thread->list_node), &running_queue);
		list_insert_first(&(new_thread->thread_list), &thread_list);
	}
	
	//cache_clean_addresses((uint32_t *)new_thread, sizeof(thread_s));
	//cache_clean_addresses((uint32_t *)new_thread->stack_base, stack_size);
	
	SCB_CleanDCache();
	
	resume_scheduler();
	
	return new_thread;
}


//--------------------------------------------------------------------------------------------------//


uint32_t* thread_stack_init(uint32_t* stack_pointer, thread_function thread_func, void* param)
{
	// Add a small offset
	stack_pointer--;
	
	
	// xPSR
	*stack_pointer-- = (uint32_t)KERNEL_THREAD_xPSR;
	
	
	// Program counter
	*stack_pointer-- = (uint32_t)thread_func;
	
	
	// Link register
	*stack_pointer = (uint32_t)kernel_delete_thread;
	
	
	// R12, R3, R2, R1, R0
	stack_pointer -= 5;
	*stack_pointer = (uint32_t)param;
	
	
	// R11, R10, R9, R8, R7, R6, R5, R4
	stack_pointer -= 8;
	
	
	return stack_pointer;
}


//--------------------------------------------------------------------------------------------------//


static void kernel_delete_thread(void)
{
	current_thread->state = THREAD_STATE_EXIT_PENDING;
	
	reschedule();
	
	while (1)
	{
		
	}
}


//--------------------------------------------------------------------------------------------------//


static void round_robin_idle_thread(void* param)
{
	while (1)
	{
		// The idle thread should not do anything
	}
}


//--------------------------------------------------------------------------------------------------//
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "scheduler.h"
#include "systick.h"
#include "interrupt.h"
#include "dynamic_memory.h"
#include "board_serial_programming.h"
#include "check.h"
#include "critical_section.h"
#include "board_serial.h"
#include "cache.h"
#include "compiler.h"
#include "gpio.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>
#include <core_cm7.h>


//--------------------------------------------------------------------------------------------------//


struct scheduler_info scheduler;


//--------------------------------------------------------------------------------------------------//


// Temporarily callback for the fast programming interface. When the flashed program
// gets terminated, this handler will delete the memory footprint of the actual code. 
void (*fast_program_interface_delete)(void);


//--------------------------------------------------------------------------------------------------//


void round_robin_scheduler(void);

void scheduler_start(void);

void round_robin_idle_thread(void* param);

void thread_stack_overflow_event(char* data);

void suspend_scheduler(void);

void resume_scheduler(void);

void reset_runtime(void);

static inline void process_expired_delays(void);


//--------------------------------------------------------------------------------------------------//


// The kernel does not use the SysTick handler for context switching
// since this would block all interrupts. Instead it is pending the PendSV
// interrupt, which has the lowest priority. This handler will take care
// of the context switching.

void SysTick_Handler()
{
	// First we check if a reschedule has occurred since the last context switch.
	// In that case the thread has not run for a full time slice. The rescheduler
	// will calculate the runtime, which will be added here, if a reschedule has occurred. 
	if (scheduler.reschedule_pending)
	{
		scheduler.reschedule_pending = 0;
		
		scheduler.tick += scheduler.reschedule_runtime;
		scheduler.current_thread->stats.new_window_time += scheduler.reschedule_runtime;
	}
	else
	{
		scheduler.tick += 1000;
		scheduler.current_thread->stats.new_window_time += 1000;
	}
	
	
	if (scheduler.tick > scheduler.tick_to_runtime)
	{
		reset_runtime();
		scheduler.tick_to_runtime = scheduler.tick + 1000000;
	}
	
	// Launch the scheduler
	round_robin_scheduler();
	
	// Pend the PendSV exception that will execute the actual context switch
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}


//--------------------------------------------------------------------------------------------------//


void kernel_launch(void)
{
	// Configure the SysTick timer
	systick_config();
	systick_set_reload_value(300000000 / KERNEL_TICK_FREQUENCY);
	
	
	scheduler.systick_divider = 300000000 / KERNEL_TICK_FREQUENCY / 1000;
	
	// Start the scheduler
	scheduler.status = SCHEDULER_STATUS_RUNNING;
	
	
	// Update the kernel tick and tick to wake
	scheduler.tick = 0;
	
	
	// Set the tick to wake variable to not trigger
	scheduler.tick_to_wake = 0xffffffffffffffff;
	scheduler.tick_to_runtime = scheduler.tick + 1000000;
	
	
	// Set the current thread to point to the first thread to run
	if (scheduler.running_queue.last != NULL)
	{
		scheduler.current_thread = scheduler.running_queue.last->object;
		
		list_remove_last(&scheduler.running_queue);
	}
	else
	{
		scheduler.current_thread = scheduler.idle_thread;
	}
	
	// Set the priorities for the SysTick and PendSV exception
	//
	// Under normal operation the SysTick exception should have the highest priority
	// and the PendSV should have the lowest. In debug mode this will make the system
	// crash. This is because the SysTick exception handler will print things to the
	// screen, and therefore not return within the new time slice. This means that
	// the scheduler runs several times without a context-switch.
	interrupt_enable_peripheral_interrupt(SysTick_IRQn, IRQ_LEVEL_0);
	interrupt_enable_peripheral_interrupt(PendSV_IRQn, IRQ_LEVEL_7);
	
	
	// This configures the kernels context switch mechanism
	// That includes configuring the new program stack
	scheduler_start();
}


//--------------------------------------------------------------------------------------------------//


void reschedule(void)
{
	scheduler.reschedule_runtime = 1000 - systick_get_counter_value() / scheduler.systick_divider;
	scheduler.reschedule_pending = 1;
	
	// Not sure is this is a good idea, since it might fuck up the timing
	systick_set_counter_value(0);
	
	// Since the scheduling algorithm is called in the SysTick exception rather than
	// in the PendSV exception we pend the SysTick handler instead of the PendSV handler. 
	SCB->ICSR |= (1 << SCB_ICSR_PENDSTSET_Pos);
}


//--------------------------------------------------------------------------------------------------//


void thread_delay(uint32_t ticks)
{
	// Suspend the scheduler since we do not want
	// a context switch inside here
	suspend_scheduler();
	
	
	// Calculate the right tick to wake
	uint32_t tmp = scheduler.tick + ticks * 1000;
	
	
	// Write the value to the thread control block
	scheduler.current_thread->tick_to_wake = tmp;
	scheduler.current_thread->list_node.value = tmp;
	
	
	// Update the next list to place the thread in
	scheduler.current_thread->next_list = &scheduler.delay_queue;
	
	
	// Let the scheduler start again
	resume_scheduler();
	
	
	// Free the resources used
	reschedule();
}


//--------------------------------------------------------------------------------------------------//


// This is the kernels scheduler which decide what thread to run next
// The next thread to run should be placed in the kernel_current_thread_pointer
// variable

void round_robin_scheduler(void)
{
	// Do not allow any context switch when the scheduler is suspended
	if (likely(scheduler.status == SCHEDULER_STATUS_RUNNING))
	{
		
		// This first part processes the thread that is done executing. This may involve
		// placing the thread in another list i.e. suspend the thread, or deleting the
		// thread if that is necessary. This part should also handler stack overflow and
		// memory overflow. 
		if (scheduler.current_thread != scheduler.idle_thread)
		{
			if (scheduler.current_thread->next_list != NULL)
			{
				// Check which list the thread is going to be placed in
				if (scheduler.current_thread->next_list == &scheduler.delay_queue)
				{
					list_insert_delay(&scheduler.current_thread->list_node, &scheduler.delay_queue);
					
					// Update the kernel tick to wake
					scheduler.tick_to_wake = ((struct thread_structure *)(scheduler.delay_queue.first->object))->tick_to_wake;
				}
				else
				{
					list_insert_first(&(scheduler.current_thread->list_node), scheduler.current_thread->next_list);
				}
				
				scheduler.current_thread->next_list = NULL;
			}
			else
			{
				
				if (unlikely(scheduler.current_thread->state == THREAD_STATE_EXIT_PENDING))
				{
					if (scheduler.current_thread->ID == 6969)
					{
						// This thread belong to the fast programming interface. So we must
						// delete the program memory buffer as well
						if (fast_program_interface_delete != NULL)
						{
							fast_program_interface_delete();
						}
					}
					
					list_remove_item(&(scheduler.current_thread->thread_list), &scheduler.threads);
					
					// Then we have to delete the memory resources
					//dynamic_memory_free(kernel_current_thread_pointer->stack_base);
					dynamic_memory_free(scheduler.current_thread);
				}
				else
				{
					list_insert_first(&(scheduler.current_thread->list_node), &scheduler.running_queue);
				}
			}
		}
		
		// Now we check if some delays has expired. Is so, all the expired threads has to be
		// moved to the running queue. This function will place the threads last in the running
		// queue, such that the are run first. 
		if (scheduler.tick_to_wake <= scheduler.tick)
		{
			process_expired_delays();
		}
		
		
		// Here we choose the next thread to run. This is normally the last element in the running queue
		if (scheduler.running_queue.last == NULL)
		{
			scheduler.next_thread = scheduler.idle_thread;
			gpio_set_pin_value(PIOC, 8);
		}
		else
		{
			scheduler.next_thread = scheduler.running_queue.last->object;
			
			list_remove_last(&scheduler.running_queue);
			gpio_clear_pin_value(PIOC, 8);
		}
		
		scheduler.next_thread->context_switches++;
	}
	else
	{
		// The scheduler is not running
	}
}


//--------------------------------------------------------------------------------------------------//


void scheduler_current_thread_to_queue(list_s* list)
{
	scheduler.current_thread->next_list = list;
}


//--------------------------------------------------------------------------------------------------//





//--------------------------------------------------------------------------------------------------//


static inline void process_expired_delays(void)
{
	list_node_s* list_iterator = scheduler.delay_queue.first;
	
	check(((struct thread_structure *)(scheduler.delay_queue.first->object))->tick_to_wake <= scheduler.tick_to_wake);
	
	uint16_t  i;
	
	for (i = 0; i < scheduler.delay_queue.size; i++)
	{
		if (((struct thread_structure *)(list_iterator->object))->tick_to_wake > scheduler.tick_to_wake)
		{
			break;
		}
		list_iterator = list_iterator->next;
	}
	
	for (uint16_t k = 0; k < i; k++)
	{
		list_node_s* tmp = scheduler.delay_queue.first;
		
		list_remove_first(&scheduler.delay_queue);
		list_insert_last(tmp, &scheduler.running_queue);
	}
	
	if (list_iterator == NULL)
	{
		// No threads left in the queue
		scheduler.tick_to_wake = 0xffffffffffffffff;
	}
	else
	{
		scheduler.tick_to_wake = ((struct thread_structure *)(scheduler.delay_queue.first->object))->tick_to_wake;
	}
}


//--------------------------------------------------------------------------------------------------//


void reset_runtime(void)
{
	scheduler.idle_thread->stats.window_time = scheduler.idle_thread->stats.new_window_time;
	scheduler.idle_thread->stats.new_window_time = 0;
	
	if (scheduler.current_thread != scheduler.idle_thread)
	{
		scheduler.current_thread->stats.window_time = scheduler.current_thread->stats.new_window_time;
		scheduler.current_thread->stats.new_window_time = 0;
	}
	
	if (scheduler.threads.size > 0)
	{
		list_node_s* list_node;
		
		list_iterate(list_node, &scheduler.threads)
		{
			if ((struct thread_structure *)(list_node->object) != scheduler.current_thread)
			{
				((struct thread_structure *)(list_node->object))->stats.window_time = ((struct thread_structure *)(list_node->object))->stats.new_window_time;
				((struct thread_structure *)(list_node->object))->stats.new_window_time = 0;
			}
		}
	}
}


//--------------------------------------------------------------------------------------------------//


void suspend_scheduler(void)
{
	(void)scheduler.status;
	scheduler.status = SCHEDULER_STATUS_SUSPENDED;
}


//--------------------------------------------------------------------------------------------------//


void resume_scheduler(void)
{
	(void)scheduler.status;
	scheduler.status = SCHEDULER_STATUS_RUNNING;
}


//--------------------------------------------------------------------------------------------------//


void scheduler_set_dynamic_loader_handler(void (*handler)(void))
{
	fast_program_interface_delete = handler;
}


//--------------------------------------------------------------------------------------------------//


void thread_stack_overflow_event(char* data)
{
	board_serial_print("Warning: Stack overflow on ");
	board_serial_print(data);
	board_serial_print(" thread\n");
}


//--------------------------------------------------------------------------------------------------//


void print_runtime_statistics(void)
{
	struct thread_structure* tmp_thread;
	
	board_serial_programming_print("Runtime\tStack\tCPU\n");
	
	int32_t cpu_usage = 1000000 - scheduler.idle_thread->stats.window_time;
	char k = cpu_usage / 10000;
	board_serial_programming_print("\t\t\t");
	board_serial_programming_write_percent(k, cpu_usage / 1000 - (k * 10));
	board_serial_programming_print(" : CPU");
	
	
	uint32_t use;
	uint32_t tot;
	
	use = dynamic_memory_get_used_size(DRAM_BANK_0);
	tot = dynamic_memory_get_total_size(DRAM_BANK_0);
	
	k = 100 * use / tot;
	uint32_t l = (100 * use) % tot;
	
	board_serial_programming_print("\t\t");
	board_serial_programming_write_percent(k, 100 * l / tot);
	board_serial_programming_print("DRAM Bank 0");
	
	use = dynamic_memory_get_used_size(DRAM_BANK_1);
	tot = dynamic_memory_get_total_size(DRAM_BANK_1);
	
	k = 100 * use / tot;
	l = (100 * use) % tot;
	
	board_serial_programming_print("\t\t");
	board_serial_programming_write_percent(k, 100 * l / tot);
	board_serial_programming_print("DRAM Bank 1");
	
	use = dynamic_memory_get_used_size(SRAM);
	tot = dynamic_memory_get_total_size(SRAM);
	
	k = 100 * use / tot;
	l = (100 * use) % tot;
	
	board_serial_programming_print("\t\t");
	board_serial_programming_write_percent(k, 100 * l / tot);
	board_serial_programming_print("SRAM");
	
	board_serial_programming_print("\n");
	
	if (scheduler.threads.size != 0)
	{
		list_node_s* node;
		
		list_iterate(node, &scheduler.threads)
		{
			tmp_thread = (struct thread_structure *)(node->object);
			
			board_serial_programming_print("%d\t\t", (uint32_t)tmp_thread->context_switches);
			
			uint32_t used_stack = tmp_thread->stack_size - ((uint32_t)tmp_thread->stack_pointer - (uint32_t)tmp_thread->stack_base);
			k = used_stack * 100 / tmp_thread->stack_size;
			l = (used_stack * 100) % tmp_thread->stack_size;
			board_serial_programming_write_percent(k, 10 * l / tmp_thread->stack_size);
			board_serial_programming_print("\t");
			
			uint8_t tmp = tmp_thread->stats.window_time / 10000;
			board_serial_programming_write_percent(tmp, tmp_thread->stats.window_time / 1000 - (tmp * 10));
			board_serial_programming_print(" : %s", tmp_thread->name);
			board_serial_programming_print("\n");
		}
	}

	board_serial_programming_print("\n\n");
}


//--------------------------------------------------------------------------------------------------//
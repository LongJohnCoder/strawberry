// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef SCHEDULER_H
#define SCHEDULER_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"
#include "config.h"
#include "list.h"
#include "spinlock.h"


//--------------------------------------------------------------------------------------------------//


// Function pointer to a thread
typedef void (*thread_function)(void *);


//--------------------------------------------------------------------------------------------------//


// Thread priorities
//
// Real time threads are scheduled with the highest priority. If not properly used
// a real time thread may block the entire system. Real time threads might be video and
// music streaming. 
//
// Interactive threads are scheduled with a lower priority than real time tasks. 
// Nevertheless the will be fast serviced in order to maintain an interactive 
// level. Interactive thread might wait on user input.
//
// Normal threads will be the most of the treads. The are scheduled fairly after 
// real time and interactive threads has executed. 
//
// Bulk thread will have long response times. These are computationally heavy CPU
// threads. Typical examples are background threads.

enum thread_priority
{
	THREAD_PRIORITY_REAL_TIME,
	THREAD_PRIORITY_INTERACTIVE,
	THREAD_PRIORITY_NORMAL,
	THREAD_PRIORITY_BULK
};



enum thread_state
{
	THREAD_STATE_SUSPENDED,
	THREAD_STATE_EXIT_PENDING,
	THREAD_STATE_RUNNING
};


//--------------------------------------------------------------------------------------------------//


struct thread_time
{
	// Total runtime in microseconds
	uint64_t					runtime;
	
	// Window runtime in microseconds
	// Typically calculated over a 1 second interval and used for runtime statistics.
	// This value will be reset every second if used and uint32_t is therefore sufficient.
	// Window time holds the microsecond runtime from the last seconds, while the new window
	// time holds the microsecond runtime from the current second. 
	uint32_t					window_time;
	uint32_t					new_window_time;
	
};


//--------------------------------------------------------------------------------------------------//


struct thread_structure
{
	// Points to the top of the stack
	uint32_t*					stack_pointer;
	
	
	// Thread ID
	uint32_t					ID;
	
	
	// Points to the next thread in the list
	struct thread_structure*	next;
	
	
	// The thread exist in two lists. The one used by the scheduler. And
	// a list that contains all the tread in the system.
	list_node_s					list_node;
	list_node_s					thread_list;
	
	
	struct List_s*				current_list;
	struct List_s*				next_list;


	// Pointer to the stack base so that we can delete the memory
	uint32_t*					stack_base;
	uint32_t					stack_size;
	
	
	// Priority of the thread
	enum thread_priority		priority;
	
	
	// Time to wake is used for the thread delay function
	uint64_t					tick_to_wake;
	
	
	// State of the thread
	enum thread_state			state;
	
	
	// This section deals with timing and stuff
	struct thread_time			time_s;
	
	
	uint64_t					context_switches;
	
	
	// Store the name of the thread
	char						name[KERNEL_THREAD_MAX_NAME_LENGTH];
	
};



struct runqueue
{
	struct spinlock lock;
	
	// This structure will hold the different run queues of the entire
	list_s real_time_queue;
	list_s normal_queue;
	list_s bulk_queue;
	
	// Blocked queues
	list_s delay_queue;
	list_s serial_queue;
};



struct scheduling_class
{
	struct scheduling_class* next;
	
	
	struct thread_structure* (*pick_next_thread)(struct runqueue* rq);
	
	
	void (*enqueue_last)	(struct runqueue* rq, struct thread_structure* thread);
	void (*enqueue_first)	(struct runqueue* rq, struct thread_structure* thread);
	void (*dequeue)			(struct runqueue* rq, struct thread_structure* thread);
	void (*yield)			(struct runqueue* rq, struct thread_structure* thread);
	void (*delay)			(struct runqueue* rq, struct thread_structure* thread);	
};


//--------------------------------------------------------------------------------------------------//


void kernel_launch(void);

void thread_config(void);

void reschedule(void);

void thread_delay(uint32_t ticks);


//--------------------------------------------------------------------------------------------------//


void print_running_queue(list_s* list);

void print_runtime_statistics(void);

void resume_scheduler(void);

void suspend_scheduler(void);

void scheduler_set_dynamic_loader_handler(void (*handler)(void));

void scheduler_current_thread_to_queue(list_s* list);


//--------------------------------------------------------------------------------------------------//


#endif
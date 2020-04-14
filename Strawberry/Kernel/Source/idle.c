// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "idle.h"
#include "scheduler.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


struct thread_structure* real_time_pick_next_thread(struct runqueue* rq);

void real_time_enqueue_last(struct runqueue* rq, struct thread_structure* thread);

void real_time_enqueue_first(struct runqueue* rq, struct thread_structure* thread);

void real_time_dequeue(struct runqueue* rq, struct thread_structure* thread);

void real_time_yield(struct runqueue* rq, struct thread_structure* thread);

void real_time_delay(struct runqueue* rq, struct thread_structure* thread);


//--------------------------------------------------------------------------------------------------//


struct scheduling_class real_time_scheduler = 
{
	.pick_next_thread	= real_time_pick_next_thread,
	.enqueue_last		= real_time_enqueue_last,
	.enqueue_first		= real_time_enqueue_first,
	.dequeue			= real_time_dequeue,
	.yield				= real_time_yield,
	.delay				= real_time_delay
};


//--------------------------------------------------------------------------------------------------//


struct thread_structure* real_time_pick_next_thread(struct runqueue* rq)
{
	return NULL;
}


//--------------------------------------------------------------------------------------------------//


void real_time_enqueue_last(struct runqueue* rq, struct thread_structure* thread);


//--------------------------------------------------------------------------------------------------//


void real_time_enqueue_first(struct runqueue* rq, struct thread_structure* thread);


//--------------------------------------------------------------------------------------------------//


void real_time_dequeue(struct runqueue* rq, struct thread_structure* thread);


//--------------------------------------------------------------------------------------------------//


void real_time_yield(struct runqueue* rq, struct thread_structure* thread);


//--------------------------------------------------------------------------------------------------//


void real_time_delay(struct runqueue* rq, struct thread_structure* thread);


//--------------------------------------------------------------------------------------------------//
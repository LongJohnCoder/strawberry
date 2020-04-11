// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef LIST_H
#define LIST_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef struct List_node_s
{
	// Pointers to the next and previous element
	struct List_node_s* next;
	struct List_node_s* prev;


	uint64_t value;


	// Pointer to the thread
	void* object;
	
} list_node_s;


//--------------------------------------------------------------------------------------------------//


typedef struct List_s
{
	// Pointer to the first and last element in the list
	struct List_node_s* first;
	struct List_node_s* last;
	
	
	// Keep track of the size of the list
	uint16_t size;
	
} list_s;


//--------------------------------------------------------------------------------------------------//


#define list_iterate(item, l) for (item = (l)->first; item; item = (item)->next)


//--------------------------------------------------------------------------------------------------//


void list_insert_first(list_node_s* list_item, list_s* list);

void list_insert_last(list_node_s* list_item, list_s* list);

void list_insert_delay(list_node_s* list_item, list_s* list);

uint8_t list_remove_first(list_s* list);

uint8_t list_remove_last(list_s* list);

uint8_t list_remove_item(list_node_s* list_item, list_s* list);


//--------------------------------------------------------------------------------------------------//


#endif
// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "list.h"
#include "check.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


// Since we a using a linked list we must assure that every element is unique. Otherwise the
// functionality will be messed up. This simple function searched a list for a match. This
// function should be called before inserting a new element.

static uint8_t kernel_list_search(list_node_s* list_item, list_s* list)
{
	if (list->first == NULL)
	{
		return 0;
	}
	
	list_node_s* list_iterator = list->first;
	
	while (list_iterator != NULL)
	{
		// If there is a match return
		if (list_item == list_iterator)
		{
			return 1;
		}
		
		list_iterator = list_iterator->next;
	}
	
	return 0;
}


//--------------------------------------------------------------------------------------------------//


void list_insert_first(list_node_s* list_item, list_s* list)
{
	// Check if the size is zero
	if (list->size == 0)
	{
		// Update the global pointers
		list->first = list_item;
		list->last = list_item;
		
		// Update the list item pointer
		list_item->next = NULL;
		list_item->prev = NULL;
	}
	else
	{
		// Search the list to make sure it do not exist
		check(kernel_list_search(list_item, list) == 0);
		
		// Update the list item pointers
		list_item->next = list->first;
		list_item->prev = NULL;
		
		// Link backwards from first node
		check(list->first->prev == NULL);
		list->first->prev = list_item;
		
		// Update the first pointer
		list->first = list_item;
	}
	
	// Increment the size
	list->size++;
}


//--------------------------------------------------------------------------------------------------//


void list_insert_last(list_node_s* list_item, list_s* list)
{
	// Check if the size is zero
	if (list->size == 0)
	{
		// Update the global pointers
		list->first = list_item;
		list->last = list_item;
		
		// Update the list item pointer
		list_item->next = NULL;
		list_item->prev = NULL;
	}
	else
	{
		// Search the list to make sure it do not exist
		check(kernel_list_search(list_item, list) == 0);
		
		// Update the list item pointers
		list_item->next = NULL;
		list_item->prev = list->last;
		
		// Link backwards from first node
		check(list->last->next == NULL);
		list->last->next = list_item;
		
		// Update the first pointer
		list->last = list_item;
	}
	
	// Increment the size
	list->size++;
}


//--------------------------------------------------------------------------------------------------//


void list_insert_delay(list_node_s* list_item, list_s* list)
{
	// Check if the size if zero
	if (list->size == 0)
	{
		// Update the global pointers
		list->first = list_item;
		list->last = list_item;
		
		// Update the list item pointer
		list_item->next = NULL;
		list_item->prev = NULL;
		
		list->size = 1;
	}
	else
	{
		// Search the list to make sure it do not exist
		check(kernel_list_search(list_item, list) == 0);
		
		// Check the tick value
		uint32_t tmp_value = list_item->value;
		
		if (tmp_value <= list->first->value)
		{
			// Insert at the beginning
			list_insert_first(list_item, list);
			
			// Increment handled
		}
		else if (tmp_value >= list->last->value)
		{
			// Insert at the end
			list_insert_last(list_item, list);
			
			// Increment handled
		}
		else
		{
			list_node_s* list_iterator = list->first->next;
			
			// Iterate though the list
			while (list_iterator != NULL)
			{
				// Check tick against the following item
				if (tmp_value < list_iterator->value)
				{
					// Insert the item behind list_iterator
					list_node_s* prev_item = list_iterator->prev;
					
					list_item->next = list_iterator;
					list_iterator->prev = list_item;
					
					prev_item->next = list_item;
					list_item->prev = prev_item;
					
					list->size++;
					
					return;
				}
				
				list_iterator = list_iterator->next;
			}
			
			// Should not reach here
			check(0);
		}
	}
}


//--------------------------------------------------------------------------------------------------//


uint8_t list_remove_first(list_s* list)
{
	if (list->size == 0)
	{
		// Nothing to remove
		return 0;
	}
	else if (list->size == 1)
	{
		// Remove the only element present
		list->first->next = NULL;
		list->first->prev = NULL;
		
		// Update the list first and last element
		list->first = NULL;
		list->last = NULL;
		
		list->size = 0;
	}
	else
	{
		// Remove the first element
		check(list->first != NULL);
		check(list->last != NULL);
		// Update the first pointer
		list->first = list->first->next;
		
		// Remove the forward link from the first element
		list->first->prev->next = NULL;
		
		// Remove the backward link from the new first item
		list->first->prev = NULL;
		
		list->size--;
	}
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t list_remove_last(list_s* list)
{
	if (list->size == 0)
	{
		// Nothing to remove
		return 0;
	}
	else if (list->size == 1)
	{
		// Remove the only element present
		list->first->next = NULL;
		list->first->prev = NULL;
		
		// Update the list first and last element
		list->first = NULL;
		list->last = NULL;
		
		list->size = 0;
	}
	else
	{
		// Remove the last element
		list->last = list->last->prev;
		
		// Remove the forward link from the first element
		list->last->next->prev = NULL;
		
		// Remove the backward link from the new first item
		list->last->next = NULL;
		
		list->size--;
	}
	
	return 1;
}


//--------------------------------------------------------------------------------------------------//


uint8_t list_remove_item(list_node_s* list_item, list_s* list)
{
	if (list->size == 0)
	{
		return 0;
	}
	else if (list_item == list->first)
	{
		return list_remove_first(list);
	}
	else if (list_item == list->last)
	{
		return list_remove_last(list);
	}
	else
	{
		check(kernel_list_search(list_item, list) == 1);
		list_item->next->prev = list_item->prev;
		list_item->prev->next = list_item->next;
		
		// Update the list_item pointers
		list_item->next = NULL;
		list_item->prev = NULL;
		
		list->size--;
		
		return 1;
	}
}


//--------------------------------------------------------------------------------------------------//
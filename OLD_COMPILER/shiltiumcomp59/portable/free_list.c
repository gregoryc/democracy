/*
Shiltiumcomp -- the first Shiltolang compiler

Copyright (C) 2010 Gregory Cohen <gregorycohen2@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
This module contains functions for the free list -- a list of pointers
to blocks that are freed after most of the work in this program is done
(i.e., after the generated program is compiled and about to run).
*/

#include "../global_headers/headers.h"
#include "../global_headers/globals.h"

attr_cold_and_noreturn static void unable_to_allocate_memory_for_a_free_list_node(void) {
	output_nullt_string("Memory error: unable to allocate memory for a FreeList node.\n");
	exit_program();
}

void free_list_append(FreeListHead* head, void* block_to_free, void (*free_function)(void* block_to_free)) {
	assert_comparison(block_to_free, !=, NULL, void*, void*);
	assert_comparison(free_function, !=, NULL, void (*)(void*), void*);
	
	if (likely(head)) {
		FreeListNode* new_free_list_node;
		
		new_free_list_node = (FreeListNode*)m_alloc(sizeof(FreeListNode));
		
		if (unlikely(!head)) unable_to_allocate_memory_for_a_free_list_node();
		
		ARROW_ASSIGN(new_free_list_node, block_to_free) = block_to_free;
		ARROW_ASSIGN(new_free_list_node, free_function) = free_function;
		ARROW_ASSIGN(new_free_list_node, next_node) = DEREF(head);
		DEREF_ASSIGN(head) = new_free_list_node;
	} else {
		DEREF_ASSIGN(head) = (FreeListNode*)m_alloc(sizeof(FreeListNode));
		
		if (unlikely(!DEREF(head))) unable_to_allocate_memory_for_a_free_list_node();
		
		ARROW_ASSIGN(DEREF(head), block_to_free) = block_to_free;
		ARROW_ASSIGN(DEREF(head), free_function) = free_function;
		ARROW_ASSIGN(DEREF(head), next_node) = NULL;
	}
}

/*
This function frees the free list.
Blocks with a non-m_free() free function get freed first.
*/
void free_list_del(FreeListHead head) {
	#if DEBUG
		u length = 0;
	#endif
	
	FreeListHead original_head = head;
	
	while (head) {
		FreeListNode* next_node = ARROW(head, next_node);
		
		if (ARROW(head, free_function) != m_free) {
			ARROW(head, free_function)(ARROW(head, block_to_free));
			
			#if DEBUG
				++length;
			#endif
		}
		
		head = next_node;
	}
	
	head = original_head;
	
	while (head) {
		FreeListNode* next_node = ARROW(head, next_node);
		
		if (ARROW(head, free_function) == m_free) {
			ARROW(head, free_function)(ARROW(head, block_to_free));
			
			#if DEBUG
				++length;
			#endif
		}
		
		m_free(head);
		head = next_node;
	}
	
	#if DEBUG
		output_nullt_string("\nFreed the free list; number of nodes = ");
		putu(length);
		output_nullt_string("\n\n");
	#endif
}

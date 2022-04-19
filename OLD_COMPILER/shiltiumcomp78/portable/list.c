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
to blocks that are freed after most of the work in this program is done,
i.e., after the generated program is compiled and about to run.
*/

/*
A singly-linked list implementation.
*/

#include "../global_headers/headers.h"

attrs_cold_and_nonnull
static void unable_to_allocate_memory_for_a_linked_list_node(void) {
	output_nullt_string("Memory error: Unable to allocate memory for a linked list node.\n");
	exit_program();
}

__attribute__((nonnull (1)))
void linked_list_append(LinkedListHead* const head, void* const value) {
	assert_comparison(value, !=, NULL, void* const, void*);
	
	if (likely(head)) {
		LinkedListNode* new_linked_list_list_node;
		
		new_linked_list_list_node = (LinkedListNode*)m_alloc(sizeof(LinkedListNode));
		
		if (unlikely(!head)) {
			unable_to_allocate_memory_for_a_linked_list_node();
		}
		
		ARROW_ASSIGN(new_linked_list_list_node, value) = value;
		ARROW_ASSIGN(new_linked_list_list_node, next_node) = DEREF(head);
		DEREF_ASSIGN(head) = new_linked_list_list_node;
	} else {
		DEREF_ASSIGN(head) = (LinkedListNode*)m_alloc(sizeof(LinkedListNode));
		
		if (unlikely(!head)) {
			unable_to_allocate_memory_for_a_linked_list_node();
		}
		
		ARROW_ASSIGN(DEREF(head), value) = value;
		ARROW_ASSIGN(DEREF(head), next_node) = NULL;
	}
}

__attribute__((nonnull (1)))
void linked_list_append_n_bytes(LinkedListHead* const head, void* const value, const u number_of_bytes) {
	assert_comparison(value, !=, NULL, void* const, void*);
	
	if (likely(head)) {
		LinkedListNode* new_linked_list_list_node;
		
		new_linked_list_list_node = (LinkedListNode*)m_alloc(sizeof(LinkedListNode) - 8 + number_of_bytes);
		
		if (unlikely(!head)) {
			unable_to_allocate_memory_for_a_linked_list_node();
		}
		
		ARROW_ASSIGN(new_linked_list_list_node, next_node) = DEREF(head);
		
		memcpy(&new_linked_list_list_node->value, value, number_of_bytes);
		DEREF_ASSIGN(head) = new_linked_list_list_node;
	} else {
		DEREF_ASSIGN(head) = (LinkedListNode*)m_alloc(sizeof(LinkedListNode) - 8 + number_of_bytes);
		
		if (unlikely(!head)) {
			unable_to_allocate_memory_for_a_linked_list_node();
		}
		
		ARROW_ASSIGN(DEREF(head), next_node) = NULL;
		memcpy(&DEREF(head)->value, value, number_of_bytes);
	}
}

void linked_list_del_with_values(LinkedListHead head) {
	while (head) {
		LinkedListNode* next_node = ARROW(head, next_node);
		m_free(ARROW(head, value));
		m_free(head);
		head = next_node;
	}
}

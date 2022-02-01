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

#include "../../global_headers/headers.h"

static void* pointer_that_points_to_null = NULL;

void hash_table_init(
	HashTable* cr ht,
	const u number_of_linked_lists,
	const u max_amount_of_items_in_a_linked_list,
	const f multiplication_amount
) {
	assert_comparison(ht, !=, NULL, HashTable*, void*);
	assert_comparison(number_of_linked_lists, >, 0, u, u);
	assert_comparison(max_amount_of_items_in_a_linked_list, >, 0, u, u);
	assert_comparison(multiplication_amount, >, 2.0, f, f);
	
	/* This hash table class uses lazy allocation. */
	ARROW_ASSIGN(ht, linked_lists) = NULL;
	ARROW_ASSIGN(ht, allocated_length) = number_of_linked_lists;
	ARROW_ASSIGN(ht, length) = 0;
	ARROW_ASSIGN(ht, max_amount_of_items_in_a_linked_list) = max_amount_of_items_in_a_linked_list;
	ARROW_ASSIGN(ht, multiplication_amount) = multiplication_amount;
}

/* The hash function for "u"s. */
attr_nonnull static u hash_u(HashTable* cr ht, const u key) {
	assert_comparison(ht, !=, NULL, HashTable*, void*);
	
	return key % ht->allocated_length;
}

/* The hash function for "f"s. */
attr_nonnull static u hash_f(HashTable* cr ht, const f key) {
	union {
		u64  u64_type;
		f    f_type;
	} f_to_u64_union;
	
	assert_comparison(ht, !=, NULL, HashTable*, void*);
	assert_comparison(sizeof(f), ==, sizeof(u64), size_t, size_t);
	
	f_to_u64_union.f_type = key;
	
	return f_to_u64_union.u64_type % ht->allocated_length;
}

/* The hash function for strings. */
attr_nonnull static u hash_string(HashTable* cr ht, const char* cr key, u length_of_key) {
	u hash;
	
	assert_comparison(ht, !=, NULL, HashTable* cr, void*);
	assert_comparison(key, !=, NULL, const char* cr, void*);
	
	/* a random prime number */
	hash = 580169;
	
	while (length_of_key--) {
		hash ^= (ARRAY_INDEX(key, length_of_key) << 12) | (hash >> 3);
	}
	
	return hash_u(ht, hash);
}

static u hash_appropriately_depending_on_type_of_key(
	HashTable* cr ht,
	const TypeUnion key,
	const u length_of_key,
	const HashType type_of_key
) {
	assert_comparison(ht, !=, NULL, HashTable*, void*);
	
	assert_comparison(
		type_of_key == HASH_TYPE_UINT ||
		type_of_key == HASH_TYPE_FLOATING_POINT_NUMBER ||
		type_of_key == HASH_TYPE_STRING,
		==, true, bool, bool
	);
	
	switch (type_of_key) {
		case HASH_TYPE_UINT:
			return hash_u(ht, key.uvalue);
		case HASH_TYPE_FLOATING_POINT_NUMBER:
			return hash_f(ht, key.fvalue);
		
		#if DEBUG
			case HASH_TYPE_STRING:
		#else
			default:
		#endif
			return hash_string(ht, key.string_value, length_of_key);
	}
	
	assert_comparison(type_of_key, !=, type_of_key, HashType, HashType);
}

__attribute__((nonnull (1)))
static void hash_table_insert_for_resize(
	HashTable* cr ht,
	const TypeUnion key,
	const u length_of_key,
	const HashType type_of_key,
	void* const value
) {
	HashItem *item, *new_head;
	u hash;
	
	assert_comparison(ht, !=, NULL, void*, void*);
	assert_comparison(key.string_value, !=, NULL, void*, void*);
	
	hash = hash_appropriately_depending_on_type_of_key(ht, key, length_of_key, type_of_key);
	
	assert_comparison(hash, <, ARROW(ht, allocated_length), u, u);
	
	item = ARRAY_INDEX(ARROW(ht, linked_lists), hash);
	
	/* Create a new node... */
	new_head = (HashItem*)m_alloc(sizeof(HashItem));
	if (unlikely(!new_head)) {
		output_nullt_string("Unable to allocate memory for a linked list in a hash table.\n");
		exit_program();
	}
	
	ARROW_ASSIGN(new_head, key) = key;
	ARROW_ASSIGN(new_head, length_of_key) = length_of_key;
	ARROW_ASSIGN(new_head, type_of_key) = type_of_key;
	ARROW_ASSIGN(new_head, value) = value;
	ARROW_ASSIGN(new_head, next_node) = ARRAY_INDEX(ARROW(ht, linked_lists), hash);
	
	/* and set it to be the head of the linked list. */
	ht->linked_lists[hash] = new_head;
}

attr_nonnull static void hash_table_resize(HashTable* cr ht) {
	u old_allocated_length;
	HashItem** old_linked_lists;
	
	assert_comparison(ht, !=, NULL, void*, void*);
	assert_comparison(ARROW(ht, allocated_length), !=, 0, u, u);
	
	old_allocated_length = ht->allocated_length;
	old_linked_lists = ht->linked_lists;
	
	ht->linked_lists = (HashItem**)m_zero_initialized_alloc(ht->allocated_length * ARROW(ht, multiplication_amount) * sizeof(HashItem*));
	
	if (ARROW(ht, linked_lists)) {
		ht->allocated_length *= ht->multiplication_amount;
	} else {
		/* Can't allocate such a big block, so try a smaller one. */
		f i;
		
		/* Python's while..else would be nice here. */
		
		for (i = ht->allocated_length / 2.0; i > 1.0; i /= 2.0) {
			ht->linked_lists = (HashItem**)m_zero_initialized_alloc(ARROW(ht, allocated_length) * sizeof(HashItem*) * i);
			if (ARROW(ht, linked_lists)) goto ok;
		}
		
		output_nullt_string("Memory error: Unable to allocate memory for hash table linked lists.\n");
		exit_program();
		
		ok:
			ARROW_ASSIGN(ht, allocated_length) *= i;
			ARROW_ASSIGN(ht, multiplication_amount) = 0;
	}
	
	while (old_allocated_length--) {
		HashItem* item = ARRAY_INDEX(old_linked_lists, old_allocated_length);
		
		while (item) {
			HashItem* next_item = ARROW(item, next_node);
			hash_table_insert_for_resize(ht, ARROW(item, key), ARROW(item, length_of_key), ARROW(item, type_of_key), ARROW(item, value));
			m_free(item);
			item = next_item;
		}
	}
	
	m_free(old_linked_lists);
}

__attribute__((nonnull (1, 7)))
static bool hash_table_insert(
	HashTable* cr ht,
	const TypeUnion key,
	const HashType type_of_key,
	const u length_of_key,
	void* const value,
	const bool replace,
	void*** cr new_value
) {
	HashItem *item, *new_head;
	u hash, times;
	
	assert_comparison(ht, !=, NULL, void*, void*);
	assert_comparison(
		type_of_key == HASH_TYPE_UINT ||
		type_of_key == HASH_TYPE_FLOATING_POINT_NUMBER ||
		type_of_key == HASH_TYPE_STRING,
		==, true, bool, bool
	);
	
	#if DEBUG
		if (type_of_key == HASH_TYPE_STRING) {
			assert_comparison(key.string_value, !=, 0, u, u);
		}
	#endif
	
	hash = hash_appropriately_depending_on_type_of_key(ht, key, length_of_key, type_of_key);
	
	assert_comparison(hash, <, ARROW(ht, allocated_length), u, u);
	
	times = 0;
	
	if (!ARROW(ht, linked_lists)) {
		ARROW_ASSIGN(ht, linked_lists) = (HashItem**)m_zero_initialized_alloc(ht->allocated_length * sizeof(HashItem*));
	}
	
	for (item = ht->linked_lists[hash]; item; item = item->next_node) {
		if (unlikely(times == ht->max_amount_of_items_in_a_linked_list && ARROW(ht, allocated_length) < 10000000)) {
			ARROW_ASSIGN(ht, max_amount_of_items_in_a_linked_list) += 6;
			hash_table_resize(ht);
			hash_table_insert_for_resize(ht, key, length_of_key, type_of_key, value);
			ARROW_ASSIGN(ht, length)++;
			return true;
		}
		
		if (type_of_key == HASH_TYPE_UINT && item->type_of_key == HASH_TYPE_UINT) {
			/* if the key is already in the hash table */
			if (key.uvalue == item->key.uvalue) {
				if (replace) {
					item->key = key;
					item->value = value;
					DEREF_ASSIGN(new_value) = &item->value;
					return true;
				} else {
					DEREF_ASSIGN(new_value) = &item->value;
					return false;
				}
			}
		} else if (type_of_key == HASH_TYPE_FLOATING_POINT_NUMBER && item->type_of_key == HASH_TYPE_FLOATING_POINT_NUMBER) {
			/* if the key is already in the hash table */
			if (key.fvalue == item->key.fvalue) {
				if (replace) {
					item->key = key;
					item->value = value;
					DEREF_ASSIGN(new_value) = &item->value;
					return true;
				} else {
					DEREF_ASSIGN(new_value) = &item->value;
					return false;
				}
			}
		} else {
			#if DEBUG
			if (type_of_key == HASH_TYPE_STRING && item->type_of_key == HASH_TYPE_STRING) {
			#endif
			/* if the key is already in the hash table */
				if (strnequal(key.string_value, item->key.string_value, length_of_key, item->length_of_key)) {
					if (replace) {
						item->key = key;
						item->length_of_key = length_of_key;
						item->value = value;
						DEREF_ASSIGN(new_value) = &item->value;
						return true;
					} else {
						DEREF_ASSIGN(new_value) = &item->value;
						return false;
					}
				}
			#if DEBUG
			} else {
				assert_comparison(0, !=, 0, u, u);
			}
			#endif
		}
		++times;
	}
	
	/* Create a new node... */
	new_head = (HashItem*)m_alloc(sizeof(HashItem));
	if (unlikely(!new_head)) {
		output_nullt_string("Unable to allocate memory for a linked list in a hash table.\n");
		exit_program();
	}
	
	ARROW_ASSIGN(new_head, key) = key;
	ARROW_ASSIGN(new_head, length_of_key) = length_of_key;
	ARROW_ASSIGN(new_head, type_of_key) = type_of_key;
	ARROW_ASSIGN(new_head, value) = value;
	ARROW_ASSIGN(new_head, next_node) = ARRAY_INDEX(ARROW(ht, linked_lists), hash);
	
	/* and set it to be the head of the linked list. */
	ARRAY_INDEX_ASSIGN(ARROW(ht, linked_lists), hash) = new_head;
	
	DEREF_ASSIGN(new_value) = &new_head->value;
	
	ARROW_ASSIGN(ht, length)++;
	return true;
}

__attribute__((nonnull (1, 6))) bool /* successfully added or replaced a new key? */
hash_table_insert_with_string_key(
	HashTable* cr ht,
	char* const key,
	const u length_of_key,
	void* const value,
	const bool replace,
	void*** cr new_value
) {
	TypeUnion tu;
	tu.string_value = key;
	return hash_table_insert(ht, tu, HASH_TYPE_STRING, length_of_key, value, replace, new_value);
}

__attribute__((nonnull (1, 5))) bool /* successfully added or replaced a new key? */
hash_table_insert_with_u_key(
	HashTable* cr ht,
	const u key,
	void* const value,
	const bool replace,
	void*** cr new_value
) {
	TypeUnion tu;
	tu.uvalue = key;
	return hash_table_insert(ht, tu, HASH_TYPE_UINT, 0, value, replace, new_value);
}

__attribute__((nonnull (1, 5))) bool /* successfully added or replaced a new key? */
hash_table_insert_with_f_key(
	HashTable* cr ht,
	const f key,
	void* const value,
	const bool replace,
	void*** cr new_value
) {
	TypeUnion tu;
	tu.fvalue = key;
	return hash_table_insert(ht, tu, HASH_TYPE_FLOATING_POINT_NUMBER, 0, value, replace, new_value);
}

attr_nonnull void** hash_table_get_value_from_string_key(
	HashTable* cr ht,
	const char* cr key,
	const u length_of_key,
	bool* cr in_hash_table
) {
	HashItem* item;
	u hash;
	
	assert_comparison(ht, !=, NULL, void*, void*);
	assert_comparison(key, !=, NULL, const void*, void*);
	assert_comparison(pointer_that_points_to_null, ==, NULL, void*, void*);
	
	/* If the hash table isn't allocated, there are no items in it; therefore, the key isn't in it. */
	if (!ht->linked_lists) {
		DEREF_ASSIGN(in_hash_table) = false;
		return &pointer_that_points_to_null;
	}
	
	hash = hash_string(ht, key, length_of_key);
	
	for (item = ht->linked_lists[hash]; item; item = item->next_node) {
		if (item->type_of_key == HASH_TYPE_STRING && strnequal(key, item->key.string_value, length_of_key, item->length_of_key)) {
			DEREF_ASSIGN(in_hash_table) = true;
			
			return &item->value;
		}
	}
	
	DEREF_ASSIGN(in_hash_table) = false;
	return &pointer_that_points_to_null;
}

attr_nonnull
void hash_table_delete_string_key(
	HashTable* cr ht,
	const char* cr key,
	const u length_of_key
) {
	HashItem *previous_node, *next_node, *node;
	u hash;
	
	assert_comparison(ht, !=, NULL, HashTable* cr, void*);
	assert_comparison(key, !=, NULL, const void*, void*);
	assert_comparison(pointer_that_points_to_null, ==, NULL, void*, void*);
	assert_comparison(ARROW(ht, linked_lists), !=, NULL, HashItem**, void*);
	
	previous_node = NULL;
	
	hash = hash_string(ht, key, length_of_key);
	
	node = ARRAY_INDEX(ARROW(ht, linked_lists), hash);
	
	while (!(
		ARROW(node, type_of_key) == HASH_TYPE_STRING &&
		strnequal(key, ARROW(node, key).string_value, length_of_key, ARROW(node, length_of_key))
	)) {
		previous_node = node;
		node = ARROW(node, next_node);
	}
	
	next_node = ARROW(node, next_node);
	
	m_free(node);
	
	if (previous_node) {
		ARROW_ASSIGN(previous_node, next_node) = next_node;
	} else {
		ARRAY_INDEX_ASSIGN(ARROW(ht, linked_lists), hash) = next_node;
	}
	
	ARROW_ASSIGN(ht, length)--;
}

attr_nonnull void** hash_table_get_value_from_u_key(HashTable* cr ht, u key, bool* cr in_hash_table) {
	HashItem* item;
	u hash;
	
	assert_comparison(ht, !=, NULL, void*, void*);
	assert_comparison(pointer_that_points_to_null, ==, NULL, void*, void*);
	
	/* If the hash table isn't allocated, there are no items in it; therefore, the key isn't in it. */
	if (!ht->linked_lists) {
		DEREF_ASSIGN(in_hash_table) = false;
		return &pointer_that_points_to_null;
	}
	
	hash = hash_u(ht, key);
	
	for (item = ht->linked_lists[hash]; item; item = item->next_node) {
		if (item->type_of_key == HASH_TYPE_UINT && item->key.uvalue == item->key.uvalue) {
			DEREF_ASSIGN(in_hash_table) = true;
			return &item->value;
		}
	}
	
	DEREF_ASSIGN(in_hash_table) = false;
	return &pointer_that_points_to_null;
}

attr_nonnull void** hash_table_get_value_from_f_key(HashTable* cr ht, f key, bool* in_hash_table) {
	HashItem* item;
	u hash;
	
	assert_comparison(ht, !=, NULL, void*, void*);
	assert_comparison(pointer_that_points_to_null, ==, NULL, void*, void*);
	
	/* If the hash table isn't allocated, there are no items in it; therefore, the key isn't in it. */
	if (!ht->linked_lists) {
		DEREF_ASSIGN(in_hash_table) = false;
		return &pointer_that_points_to_null;
	}
	
	hash = hash_f(ht, key);
	
	for (item = ARRAY_INDEX(ARROW(ht, linked_lists), hash); item; item = ARROW(item, next_node)) {
		if (item->type_of_key == HASH_TYPE_FLOATING_POINT_NUMBER && item->key.fvalue == ARROW(item, key).fvalue) {
			DEREF_ASSIGN(in_hash_table) = true;
			return &item->value;
		}
	}
	
	DEREF_ASSIGN(in_hash_table) = false;
	return &pointer_that_points_to_null;
}

void hash_iterator_init(HashTable* const restrict hash_table, HashIterator* cr hash_iterator) {
	assert_comparison(hash_table, !=, NULL, void*, void*);
	assert_comparison(hash_iterator, !=, NULL, void*, void*);
	
	ARROW_ASSIGN(hash_iterator, hash_table) = hash_table;
	ARROW_ASSIGN(hash_iterator, current_hashitem) = NULL;
	ARROW_ASSIGN(hash_iterator, how_far_in) = -1;
}

bool /* not done iterating */ hash_iterator_next(HashIterator* cr iterator) {
	assert_comparison(iterator, !=, NULL, void*, void*);
	assert_comparison(ARROW(iterator, hash_table), !=, NULL, void*, void*);
	
	/* If the hash table isn't allocated, there are zero items in it, and therefore we are done iterating. */
	if (!ARROW3(iterator, hash_table, linked_lists)) {
		return false;
	}
	
	while (1) {
		if (ARROW(iterator, current_hashitem)) {
			ARROW_ASSIGN(iterator, key) = ARROW(ARROW(iterator, current_hashitem), key);
			iterator->length_of_key = iterator->current_hashitem->length_of_key;
			iterator->type_of_key = iterator->current_hashitem->type_of_key;
			iterator->value = &iterator->current_hashitem->value;
			
			iterator->current_hashitem = iterator->current_hashitem->next_node;
			
			return true;
		} else {
			++iterator->how_far_in;
			if (iterator->how_far_in == iterator->hash_table->allocated_length) return false;
			iterator->current_hashitem = iterator->hash_table->linked_lists[iterator->how_far_in];
		}
	}
}

attr_nonnull
void hash_table_del_with_keys_and_values(
	HashTable* cr ht,
	void (*value_deletion_function)(const TypeUnion key, const u length_of_key, void* value)
) {
	u i;
	
	assert_comparison(ht, !=, NULL, HashTable* cr, void*);
	
	/* If the hash table isn't allocated, don't free it. */
	if (!ARROW(ht, linked_lists)) return;
	
	i = ARROW(ht, allocated_length);
	
	while (i--) {
		HashItem* item = ht->linked_lists[i];
		while (item) {
			HashItem* next_item = item->next_node;
			value_deletion_function(ARROW(item, key), ARROW(item, length_of_key), ARROW(item, value));
			m_free(ARROW(item, key).void_pointer_value);
			m_free(item);
			item = next_item;
		}
	}
	
	m_free(ARROW(ht, linked_lists));
}

attr_nonnull void hash_table_del(HashTable* cr ht) {
	u i;
	
	assert_comparison(ht, !=, NULL, HashTable* cr, void*);
	
	/* If the hash table isn't allocated, don't free it. */
	if (!ARROW(ht, linked_lists)) return;
	
	i = ARROW(ht, allocated_length);
	
	while (i--) {
		HashItem* item = ht->linked_lists[i];
		while (item) {
			HashItem* next_item = item->next_node;
			m_free(item);
			item = next_item;
		}
	}
	
	m_free(ARROW(ht, linked_lists));
}

/*
Hash table functions.
*/

#include "headers.h"

void hash_table_new(
	HashTable* const restrict ht, const u64 number_of_linked_lists, const u64 max_amount_of_items_in_a_linked_list, const f64 multiplication_amount
) {
	assert_comparison(ht, !=, NULL);
	assert_comparison(number_of_linked_lists, >, 0);
	assert_comparison(max_amount_of_items_in_a_linked_list, >, 0);
	assert_comparison(multiplication_amount, >, 0.000001);
	
	/* This hash table class uses lazy initial allocation. */
	ht->linked_lists = NULL;
	ht->allocated_length = number_of_linked_lists;
	ht->length = 0;
	ht->max_amount_of_items_in_a_linked_list = max_amount_of_items_in_a_linked_list;
	ht->multiplication_amount = multiplication_amount;
}

static u64 hash_u64(HashTable* const restrict ht, const u64 key) {
	return key % ht->allocated_length;
}

/* The hash function. */
__attribute__((nonnull)) static u64 hash_string(HashTable* const restrict ht, const char* key, u64 length_of_key) {
	u64 number, additional_amount;
	
	assert_comparison(ht, !=, NULL);
	assert_comparison(key, !=, NULL);
	
	number = 1;
	additional_amount = 0;
	
	while (length_of_key--) {
		/* The number 171121 was chosen randomly, but it works well. */
		number *= (key[length_of_key] + (additional_amount += 171121));
	}
	return hash_u64(ht, number);
}

static u64 hash_appropriately_depending_on_type_of_key(HashTable* const restrict ht, const TypeUnion key, const u64 length_of_key, const HashType type_of_key) {
	if (type_of_key == HASH_TYPE_UINT) {
		return hash_u64(ht, key.u64_value);
	}
	
	return hash_string(ht, key.string_value, length_of_key);
}

__attribute__((nonnull)) static void hash_table_insert_for_resize(HashTable* ht, TypeUnion key, u64 length_of_key, HashType type_of_key, void* value) {
	HashItem *item, *new_head;
	u64 hash;
	
	assert_comparison(ht, !=, NULL);
	assert_comparison(key.string_value, !=, NULL);
	assert_comparison(value, !=, NULL);
	
	hash = hash_appropriately_depending_on_type_of_key(ht, key, length_of_key, type_of_key);
	
	for (item = ht->linked_lists[hash]; item; item = item->next) {
		/* If the key is already in the hash table... */
		if (strnequal(key.string_value, item->key.string_value, length_of_key, item->length_of_key)) {
			/* set the value to the new value and return the new value. */
			item->value = value;
		}
	}
	
	/* Create a new node... */
	new_head = (HashItem*)m_alloc(sizeof(HashItem));
	new_head->key = key;
	new_head->length_of_key = length_of_key;
	new_head->type_of_key = type_of_key;
	new_head->value = value;
	new_head->next = ht->linked_lists[hash];
	
	/* and set it to be the head of the linked list. */
	ht->linked_lists[hash] = new_head;
}

__attribute__((nonnull)) static void hash_table_resize(HashTable* ht) {
	u64 old_allocated_length;
	HashItem** old_linked_lists;
	
	assert_comparison(ht, !=, NULL);
	
	old_allocated_length = ht->allocated_length;
	old_linked_lists = ht->linked_lists;
	ht->linked_lists = (HashItem**)m_zero_initialized_alloc(ht->allocated_length * sizeof(HashItem*));
	
	if (ht->linked_lists) {
		ht->allocated_length *= ht->multiplication_amount;
	} else {
		/* Can't allocate such a big block, so try a smaller one. */
		f64 i;
		
		for (i = ht->allocated_length / 2.0; i > 1.0; i /= 2.0) {
			ht->linked_lists = (HashItem**)m_zero_initialized_alloc(ht->allocated_length * sizeof(HashItem*) * i);
		}
		
		ht->allocated_length *= i;
		ht->multiplication_amount = 0;
	}
	
	while (old_allocated_length--) {
		HashItem* item;
		for (item = old_linked_lists[old_allocated_length]; item;) {
			HashItem* next_item = item->next;
			hash_table_insert_for_resize(ht, item->key, item->length_of_key, item->type_of_key, item->value);
			m_free(item);
			item = next_item;
		}
	}
	m_free(old_linked_lists);
}

__attribute__((nonnull))
static bool hash_table_insert(HashTable* const restrict ht, TypeUnion key, HashType type_of_key, u64 length_of_key, void* value, const bool replace) {
	HashItem *item, *new_head;
	u64 hash, times;
	
	assert_comparison(ht, !=, NULL);
	assert_comparison(type_of_key == HASH_TYPE_UINT || key.string_value, !=, 0);
	assert_comparison(value, !=, NULL);
	
	hash = hash_appropriately_depending_on_type_of_key(ht, key, length_of_key, type_of_key);
	
	times = 0;
	
	if (!ht->linked_lists) {
		ht->linked_lists = (HashItem**)m_zero_initialized_alloc(ht->allocated_length * sizeof(HashItem*));
	}
	
	for (item = ht->linked_lists[hash]; item; item = item->next) {
		if (times == ht->max_amount_of_items_in_a_linked_list) {
			ht->max_amount_of_items_in_a_linked_list += 10;
			#if DEBUG
				output_nullt_string("(in hash_table_insert()) resize from ");
				output_u64(ht->allocated_length);
				output_nullt_string(" to ");
				output_u64(ht->allocated_length * ht->multiplication_amount);
				output_nullt_string(".\n");
			#endif
			hash_table_resize(ht);
			hash_table_insert_for_resize(ht, key, length_of_key, type_of_key, value);
			++ht->length;
			return true;
		}
		if (type_of_key == HASH_TYPE_UINT && item->type_of_key == HASH_TYPE_UINT) {
			/* if the key is already in the hash table */
			if (key.u64_value == item->key.u64_value) {
				if (replace) {
					item->key = key;
					item->value = value;
					return true;
				} else {
					return false;
				}
			}
		} else if (type_of_key == HASH_TYPE_UINT && item->type_of_key == HASH_TYPE_UINT) {
			/* if the key is already in the hash table */
			if (strnequal(key.string_value, item->key.string_value, length_of_key, item->length_of_key)) {
				if (replace) {
					item->key = key;
					item->length_of_key = length_of_key;
					item->value = value;
					return true;
				} else {
					return false;
				}
			}
		}
		++times;
	}
	
	/* Create a new node... */
	new_head = (HashItem*)m_alloc(sizeof(HashItem));
	new_head->key = key;
	new_head->length_of_key = length_of_key;
	new_head->type_of_key = type_of_key;
	new_head->value = value;
	new_head->next = ht->linked_lists[hash];
	
	/* and set it to be the head of the linked list. */
	ht->linked_lists[hash] = new_head;
	
	++ht->length;
	return true;
}

bool hash_table_insert_string(HashTable* const restrict ht, char* key, const u64 length_of_key, void* value, const bool replace) {
	TypeUnion tu;
	tu.string_value = key;
	return hash_table_insert(ht, tu, HASH_TYPE_STRING, length_of_key, value, replace);
}

bool hash_table_insert_uint(HashTable* const restrict ht, const u64 key, void* value, const bool replace) {
	TypeUnion tu;
	tu.u64_value = key;
	return hash_table_insert(ht, tu, HASH_TYPE_UINT, 0, value, replace);
}

void** hash_table_get_value_with_string_key(HashTable* ht, const char* key, u64 length_of_key) {
	HashItem* item;
	u64 hash;
	
	static void* pointer_that_points_to_null = NULL;
	
	assert_comparison(ht, !=, NULL);
	assert_comparison(key, !=, NULL);
	
	/* If the hash table isn't allocated, there are no items in it; therefore, the key isn't in it. */
	if (!ht->linked_lists) {
		return &pointer_that_points_to_null;
	}
	
	hash = hash_string(ht, key, length_of_key);
	
	for (item = ht->linked_lists[hash]; item; item = item->next) {
		/* If the key is in the hash table... */
		if (item->type_of_key == HASH_TYPE_STRING && strnequal(key, item->key.string_value, length_of_key, item->length_of_key)) {
			/* then return the value. */
			return &item->value;
		}
	}
	
	/* Otherwise, return the addresss of a static pointer that points to NULL. */
	return &pointer_that_points_to_null;
}

void** hash_table_get_value_with_uint_key(HashTable* ht, u64 key) {
	HashItem* item;
	u64 hash;
	
	static void* pointer_that_points_to_null = NULL;
	
	assert_comparison(ht, !=, NULL);
	
	/* If the hash table isn't allocated, there are no items in it; therefore, the key isn't in it. */
	if (!ht->linked_lists) {
		return &pointer_that_points_to_null;
	}
	
	hash = hash_u64(ht, key);
	
	for (item = ht->linked_lists[hash]; item; item = item->next) {
		/* If the key is in the hash table... */
		if (item->type_of_key == HASH_TYPE_UINT && item->key.u64_value == item->key.u64_value) {
			/* then return the value. */
			return &item->value;
		}
	}
	
	/* Otherwise, return the addresss of a static pointer that points to NULL. */
	return &pointer_that_points_to_null;
}

void hash_iterator_init(HashTable* const restrict hash_table, HashIterator* const restrict hash_iterator) {
	assert_comparison(hash_table, !=, NULL);
	assert_comparison(hash_iterator, !=, NULL);
	
	hash_iterator->hash_table = hash_table;
	hash_iterator->current_hashitem = NULL;
	hash_iterator->how_far_in = (u64)-1;
}

bool /* not done iterating */ hash_iterator_next(HashIterator* const restrict iterator) {
	assert_comparison(iterator, !=, NULL);
	assert_comparison(iterator->hash_table, !=, NULL);
	
	/* If the hash table isn't allocated, there are zero items in it, and therefore we are done iterating. */
	if (!iterator->hash_table->linked_lists) {
		return false;
	}
	
	while (1) {
		if (iterator->current_hashitem) {
			iterator->key = iterator->current_hashitem->key;
			iterator->length_of_key = iterator->current_hashitem->length_of_key;
			iterator->type_of_key = iterator->current_hashitem->type_of_key;
			iterator->value = &iterator->current_hashitem->value;
			
			iterator->current_hashitem = iterator->current_hashitem->next;
			
			return true;
		} else {
			++iterator->how_far_in;
			if (iterator->how_far_in == iterator->hash_table->allocated_length) return false;
			iterator->current_hashitem = iterator->hash_table->linked_lists[iterator->how_far_in];
		}
	}
}

void hash_table_del(HashTable* const restrict ht) {
	u64 i;
	
	assert_comparison(ht, !=, NULL);
	
	/* If the hash table isn't allocated, don't free it. */
	if (!ht->linked_lists) return;
	
	i = ht->allocated_length;
	
	while (i--) {
		HashItem* item = ht->linked_lists[i];
		while (item) {
			HashItem* next_item = item->next;
			m_free(item);
			item = next_item;
		}
	}
	m_free(ht->linked_lists);
}

/* Deletes the hash table and all the values contained therein. */
void hash_table_del_with_values(HashTable* const restrict ht, void (*value_deletion_function)(void*)) {
	u64 i;
	
	assert_comparison(ht, !=, NULL);
	
	/* If the hash table isn't allocated, don't free it. */
	if (!ht->linked_lists) return;
	
	i = ht->allocated_length;
	
	while (i--) {
		HashItem* item = ht->linked_lists[i];
		while (item) {
			HashItem* next_item = item->next;
			value_deletion_function(item->value);
			m_free(item->value);
			m_free(item);
			item = next_item;
		}
	}
	m_free(ht->linked_lists);
}

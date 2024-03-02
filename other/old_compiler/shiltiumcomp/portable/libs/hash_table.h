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

typedef struct HashNode {
	TypeUnion key;
	u length_of_key;
	HashType type_of_key;
	void* value;
	struct HashNode* next_node;
} HashItem;

typedef struct {
	HashItem** linked_lists;
	u allocated_length;
	u length;
	u max_amount_of_items_in_a_linked_list;
	f multiplication_amount;
} HashTable;

typedef struct {
	HashItem* current_hashitem;
	HashTable* hash_table;
	u how_far_in;
	TypeUnion key;
	u length_of_key;
	HashType type_of_key;
	void** value;
} HashIterator;


void hash_table_init(
	HashTable* cr ht,
	const size_t number_of_linked_lists,
	const size_t max_amount_of_items_in_a_linked_list,
	const f multiplication_amount
) attr_nonnull;

bool hash_table_insert_with_string_key(
	HashTable* cr ht,
	char* key,
	const u length_of_key,
	void* const value,
	const bool replace,
	void*** cr new_value
) __attribute__((nonnull (1, 6)));

bool hash_table_insert_with_u_key(
	HashTable* cr ht,
	const u key,
	void* value,
	const bool replace,
	void*** cr new_value
) __attribute__((nonnull (1, 5)));

bool hash_table_insert_with_f_key(
	HashTable* cr ht,
	const f key,
	void* value,
	const bool replace,
	void*** cr new_value
) __attribute__((nonnull (1, 5)));

void** hash_table_get_value_from_string_key(
	HashTable* ht,
	const char* key,
	u length_of_key,
	bool* cr in_hash_table
) attr_nonnull;

void** hash_table_get_value_from_u_key(HashTable* ht, u key, bool* cr in_hash_table) attr_nonnull;
void** hash_table_get_value_from_f_key(HashTable* cr ht, f key, bool* cr in_hash_table) attr_nonnull;
void hash_table_delete_string_key(HashTable* cr ht, const char* cr key, const u length_of_key) attr_nonnull;
void hash_iterator_init(HashTable* cr hash_table, HashIterator* cr hash_iterator) attr_nonnull;
bool hash_iterator_next(HashIterator* cr iterator) attr_nonnull;
void hash_table_del(HashTable* cr ht) attr_nonnull;

void hash_table_del_with_keys_and_values(
	HashTable* cr ht,
	void (*value_deletion_function)(const TypeUnion key, const u length_of_key, void* value)
) attr_nonnull;

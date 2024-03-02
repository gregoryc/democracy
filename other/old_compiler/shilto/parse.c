// Last update April 3, 2010 by Gregory Cohen

#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAST_TO(offset, type) ((type)(memory.block + (offset)))

static struct {
	char *block;
	size_t used_length, length;
} memory;

#define DEFAULT_MEMORY_ALLOCATION_AMOUNT 560

static inline void memory_init(void) {
	memory.block = calloc(1, DEFAULT_MEMORY_ALLOCATION_AMOUNT);
	memory.used_length = sizeof(size_t);
	memory.length = DEFAULT_MEMORY_ALLOCATION_AMOUNT;
}

static inline void memory_del(void) {
	free(memory.block);
	memory.block = NULL;
	memory.used_length = 0;
	memory.length = 0;
}

static size_t alloc(size_t length, unsigned char alignment) {
	if (!memory.length) goto error;
	if (memory.block + memory.used_length + length < memory.block + memory.used_length) goto error;
	if (memory.block + memory.used_length + length > memory.block + memory.length) {
		memory.length = (memory.length + length + alignment) * 1.5;
		memory.block = realloc(memory.block, memory.length);
		if (!memory.block) goto error;
	}
	size_t x = memory.used_length % alignment;
	if (x) {
		memory.used_length += alignment - x;
	}
	size_t old_used_length = memory.used_length;
	memory.used_length += length;
	return old_used_length;
	
	error:
	fprintf(stderr, "alloc failed");
	exit(1);
}

/*
	if type == 0, it's an element
	if type == 1, it's a StringNode
	if type == 2, it's a string
*/

typedef struct {
	size_t
	type, length_of_block,
	parent_node, previous_node, next_node;
} Node;

typedef struct {
	size_t
	type, length_of_block,
	
	parent_node, previous_node, next_node,
	first_child_node, last_child_node, child_list_length,
	first_options_node, last_options_node, options_list_length,
	
	name, id;
} Element;

typedef struct {
	size_t
	type, length,
	parent_node, previous_node, next_node;
	
	char string[];
} StringNode;

typedef struct {
	size_t type, length;
	char string[];
} String;

static size_t element_new(void) {
	size_t element = alloc(sizeof(Element), sizeof(size_t));
	Element* e = CAST_TO(element, Element*);
	e->type = 0;
	e->length_of_block = sizeof(size_t) * 11;
	e->parent_node = 0;
	e->previous_node = 0;
	e->next_node = 0;
	e->first_child_node = 0;
	e->last_child_node = 0;
	e->child_list_length = 0;
	e->first_options_node = 0;
	e->last_options_node = 0;
	e->options_list_length = 0;
	e->name = 0;
	e->id = 0;
	return element;
}

static size_t string_node_new(void) {
	size_t string_node = alloc(sizeof(StringNode), sizeof(size_t));
	StringNode* sn = CAST_TO(string_node, StringNode*);
	sn->type = 1;
	sn->length = 0;
	sn->parent_node = 0;
	sn->previous_node = 0;
	sn->next_node = 0;
	return string_node;
}

static size_t string_new(void) {
	size_t string = alloc(sizeof(StringNode), sizeof(size_t));
	String* s = CAST_TO(string, String*);
	s->type = 2;
	s->length = 0;
	return string;
}

static void string_node_append_char(size_t string_node, char chr) {
	size_t x = alloc(1, 1);
	memory.block[x] = chr;
	++CAST_TO(string_node, StringNode*)->length;
}

static inline void debug_string(size_t string_node) {
	StringNode* x = CAST_TO(string_node, StringNode*);
	putchar('"');
	if (x->length) {
		fwrite_unlocked(x->string, x->length, 1, stdout);
	}
	putchar('"');
}

static inline size_t element_get_first_child_node(size_t element) {
	return CAST_TO(element, Element*)->first_child_node;
}

static inline size_t element_get_last_child_node(size_t element) {
	return CAST_TO(element, Element*)->last_child_node;
}


static void node_get_next_node(size_t* node) {
	*node = CAST_TO(*node, Node*)->next_node;
}

static void debug_element(size_t element, size_t recursion_level) {
	putchar('{');
	debug_string(CAST_TO(element, Element*)->name);
	size_t node = element_get_first_child_node(element);
	
	while (node) {
		putchar(' ');
		if (CAST_TO(node, Node*)->type) {
			debug_string(node);
		} else {
			debug_element(node, recursion_level + 1);
		}
		node_get_next_node(&node);
	}
	putchar('}');
	if (!recursion_level) putchar('\n');
}

static void node_append(size_t element, size_t node) {
	if (!CAST_TO(element, Element*)->first_child_node) {
		CAST_TO(element, Element*)->first_child_node = node;
		CAST_TO(element, Element*)->last_child_node = node;
		return;
	}
	size_t old_last_node = element_get_last_child_node(element);
	Node* n = CAST_TO(node, Node*);
	Node* old_last_n = CAST_TO(old_last_node, Node*);
	
	n->previous_node = old_last_node;
	old_last_n->next_node = node;
	n->next_node = 0;
	CAST_TO(element, Element*)->last_child_node = node;
}

static inline void node_set_parent(size_t node, size_t parent) {
	CAST_TO(node, Node*)->parent_node = parent;
}

static inline size_t string_get_length(size_t string) {
	return CAST_TO(string, String*)->length;
}

static char string_node_get_signed_offset(size_t string_node, long offset) {
	StringNode* x = CAST_TO(string_node, StringNode*);
	if (offset < 0) {
		return x->string[x->length + offset];
	}
	return x->string[offset];
}

static size_t parse(unsigned short recursion_level) {
	size_t e = element_new();
	size_t s = string_new();
	CAST_TO(e, Element*)->name = s;
	char chr;
	unsigned char counter_in_tag_name = 0, counter_in_body = 0;
	size_t x = 0;
	while (1) {
		chr = getchar();
		if (chr == EOF) {
			fprintf(stderr, "Syntax error\n");
			exit(1);
		}
		if (chr == '}') {
			break;
		}
		if (chr == '{') {
			if (counter_in_body) {
				if (recursion_level == 65535) {
					fprintf(stderr, "There are over 65536 levels of elements on this page!\n");
					exit(1);
				}
				ungetc(chr, stdin);
				size_t child = parse(recursion_level + 1);
				node_set_parent(child, e);
				node_append(e, child);
			} else {
				counter_in_tag_name = 1;
			}
		} else if (counter_in_tag_name) {
			if (chr == ' ' || chr == '\t' || chr == '\n') {
				counter_in_body = 1;
				counter_in_tag_name = 0;
			} else {
				string_node_append_char(s, chr);
			}
		} else if (counter_in_body) {
			if (chr == ' ' || chr == '\t' || chr == '\n') {
				if (!x) continue;
				char c = string_node_get_signed_offset(x, -1);
				if (!string_get_length(x) || c == ' ' || c == '\t' || c == '\n') continue;
				chr = ' ';
			}
			if (counter_in_body == 1) {
				x = string_node_new();
				node_set_parent(x, e);
				node_append(e, x);
				++counter_in_body;
			}
			string_node_append_char(x, chr);
		}
	}
	return e;
}

int main() {
	memory_init();
	
	size_t element = parse(0);
	debug_element(element, 0);
	
	memory_del();
	return 0;
}

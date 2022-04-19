#include "os_detection.h"
#if UNIX
	#include <sys/mman.h>
#endif

#include <unistd.h>
#define MEMORY_ALLOCATION_MODULE
#include "headers.h"

#if DEBUG
	static u64 number_of_bytes_allocated = 0;
	static u64 number_of_allocations = 0;
	static u64 number_of_frees = 0;
#endif

#if UNIX
	void* m_allocate_writable_and_executable_memory(DataAndCode* const restrict dac, size_t length) {
		void* restrict memory;
		u64 old_code_length;
		u64 page_size;
		
		assert_comparison(dac, !=, NULL);
		
		page_size = sysconf(_SC_PAGE_SIZE);
		
		#if DEBUG
			output_nullt_string("Page size is ");
			output_u64(page_size);
			output_nullt_string(" bytes.\n");
		
			output_nullt_string("Original length of code size is ");
			output_u64(ARROW(dac, code_length));
			output_nullt_string(" bytes.\n");
		#endif
		
		old_code_length = length;
		ARROW_ASSIGN(dac, code_length) = ARROW(dac, code_length) % page_size;
		if (ARROW(dac, code_length)) {
			ARROW_ASSIGN(dac, code_length) = old_code_length + (page_size - ARROW(dac, code_length));
			#if DEBUG
				output_nullt_string("Rounding up to next multiple of PAGE_SIZE: ");
			#endif
			#if DEBUG
				output_u64(ARROW(dac, code_length));
			#endif
		} else {
			#if DEBUG
				output_nullt_string("Already is a multiple of PAGE_SIZE.");
			#endif
			ARROW_ASSIGN(dac, code_length) = old_code_length;
		}
		#if DEBUG
			output_newline();
		#endif
		
		#if DEBUG
			++number_of_allocations;
			number_of_bytes_allocated += ARROW(dac, code_length);
			
			if (number_of_frees > number_of_allocations) {
				output_nullt_string("More frees than allocations!\n");
				exit(1);
			}
		#endif
		
		memory = mmap(NULL, ARROW(dac, code_length), PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (unlikely(memory == MAP_FAILED)) {
			output_nullt_string("Unable to mmap: ");
			output_errno_string();
			return (void*)-1;
			output_newline();
		}
		
		assert_comparison(memory, !=, (void*)-1);
		
		return memory;
	}
#elif WINDOWS
	void* m_allocate_writable_and_executable_memory(DataAndCode* const restrict dac, size_t length) {
		(void)dac;
		(void)length;
		
		output_nullt_string("m_allocate_writable_and_executable_memory() called\n");
		exit(1);
	}
#endif

#if UNIX
	void m_make_memory_only_executable(void* restrict address, const size_t length) {
		if (mprotect(address, length, PROT_EXEC) == -1) {
			output_nullt_string("mprotect() failed: ");
			output_errno_string();
			output_nullt_string("\nContinuing " PROGRAM_NAME " nevertheless...\n");
		}
	}
#elif WINDOWS
	void m_make_memory_only_executable(void* restrict address, const size_t length) {
		(void)address;
		(void)length;
		
		output_nullt_string("m_make_memory_only_executable() called\n");
		exit(1);
	}
#endif

#if UNIX
	void m_free_memory_for_code(void* restrict address, const size_t length) {
		#if DEBUG
			++number_of_frees;
			
			if (number_of_frees > number_of_allocations) {
				output_nullt_string("More frees than allocations!\n");
				exit(1);
			}
		#endif
		
		if (munmap(address, length) == -1) {
			output_nullt_string("munmap() failed: ");
			output_errno_string();
			output_nullt_string("\nContinuing " PROGRAM_NAME " nevertheless...\n");
		}
	}
#else
	void m_free_memory_for_code(void* restrict address, const size_t length) {
		(void)address;
		(void)length;
		
		output_nullt_string("m_free_memory_for_code() called\n");
		exit(1);
	}
#endif

#if DEBUG

/* Needs to be called the same number of times as fopen is called in the entire program. */
void update_memory_allocation_for_fopen(void) {
	++number_of_allocations;
	number_of_bytes_allocated += 568;
}

/* Needs to be called the same number of times as fopen is called in the entire program. */
void update_memory_allocation_for_fclose(void) {
	++number_of_frees;
}

char* m_alloc(size_t size) {
	++number_of_allocations;
	number_of_bytes_allocated += size;
	return (char*)malloc(size);
}

char* m_zero_initialized_alloc(size_t size) {
	++number_of_allocations;
	number_of_bytes_allocated += size;
	return (char*)calloc(1, size);
}

char* m_realloc(void* ptr, size_t size) {
	++number_of_frees;
	++number_of_allocations;
	number_of_bytes_allocated += size;
	return (char*)realloc(ptr, size);
}

void m_free(void* const restrict ptr) {
	assert_comparison(ptr, !=, NULL);
	
	++number_of_frees;
	
	if (number_of_frees > number_of_allocations) {
		output_nullt_string("More frees than allocations!\n");
		exit(1);
	}
	free(ptr);
}

void show_memory_usage(void) {
	++number_of_frees;
	
	output_newline();
	
	if (number_of_frees < number_of_allocations) {
		output_u64(number_of_allocations - number_of_frees);
		output_nullt_string(" block");
		if (number_of_allocations - number_of_frees == 1) {
			output_nullt_string(" was");
		} else {
			output_nullt_string("s were");
		}
		output_nullt_string(" not freed.\n");
	} else if (number_of_frees > number_of_allocations) {
		output_nullt_string("There are more frees than allocs! This is definitely a bug.\n");
	} else {
		output_nullt_string("All blocks were freed.\n");
	}
	
	output_nullt_string("Heap usage: ");
	output_u64(number_of_allocations);
	output_nullt_string(" alloc");
	if (number_of_allocations != 1) {
		output_char('s');
	}
	output_nullt_string(", ");
	output_u64(number_of_frees);
	output_nullt_string(" free");
	if (number_of_frees != 1) {
		output_char('s');
	}
	output_nullt_string(", ");
	output_u64(number_of_bytes_allocated);
	output_nullt_string(" byte");
	if (number_of_bytes_allocated != 1) {
		output_char('s');
	}
	output_nullt_string(" allocated\n");
}
#else
	/* This is here to prevent gcc from giving an error. */
	typedef int int_;
#endif

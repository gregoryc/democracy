/*
Memory-related functions.

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

#include "../global_headers/os_detection.h"

#if UNIX
	#include <sys/mman.h>
	#include <unistd.h>
	
	#ifdef __cplusplus
		#include <cerrno>
	#else
		#include <errno.h>
	#endif
#elif WINDOWS
	#include <windows.h>
	
	/*
	For some reason, these functions are not defined in <windows.h>
	or <malloc.h>, so they have to be defined here.
	*/
	void* _aligned_malloc(size_t size, size_t alignment);
	void _aligned_free(void* memblock);
#else
	#error "FIXME"
#endif

#define MEMORY_ALLOCATION_MODULE
#include "../global_headers/headers.h"

#if DEBUG
	static u number_of_bytes_allocated = 0;
	static u number_of_allocations = 0;
	static u number_of_frees = 0;
#endif

#if DEBUG
	static void more_frees_than_allocs_error(void) {
		output_nullt_string("There are more frees than allocs! This is definitely a bug.\n");
		exit_program();
	}
#endif

#if DEBUG
	#define VERBOSE_ALLOCATIONS_AND_FREES   0
	
	#if VERBOSE_ALLOCATIONS_AND_FREES
		#undef printf
		#undef puts
	#endif
#endif

#if UNIX
	void* m_allocate_code_section(size_t length) {
		char* memory;
		
		#if !USE_MMAP
			char* beginning_page;
			char* ending_page;
		#endif
		
		assert_comparison(length, !=, 0, u, u);
		
		#if USE_MMAP
			#if DEBUG
				++number_of_allocations;
				number_of_bytes_allocated += length;
			#endif
			
			memory = (char*)mmap(NULL, length, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
			
			if (unlikely(memory == MAP_FAILED)) {
				if (errno == ENOMEM) {
					output_nullt_string("Memory error: unable to allocate page-aligned memory (mmap() failed): ");
				} else {
					output_nullt_string("mmap() failed: ");
				}
				
				output_errno_string();
				output_newline();
				
				exit_program();
			}
			
			assert_comparison((u)memory % 4096, ==, 0, u, u);
		#else
			memory = m_alloc(length);
			
			if (unlikely(!memory)) {
				output_nullt_string("Memory error: unable to allocate memory for a page aligned block\n");
				exit_program();
			}
			
			beginning_page = memory - ((u)memory % 4096);
			assert_comparison((u)beginning_page % 4096, ==, 0, u, u);
			
			ending_page = (memory + length) + (4096 - (u)(memory + length) % 4096);
			assert_comparison((u)ending_page % 4096, ==, 0, u, u);
			
			assert_comparison((ending_page - beginning_page) % 4096, ==, 0, u, u);
			
			if (unlikely(mprotect(beginning_page, ending_page - beginning_page, PROT_READ | PROT_WRITE | PROT_EXEC) == -1)) {
				output_nullt_string("mprotect() failed: ");
				output_errno_string();
				output_newline();
				exit_program();
			}
		#endif
		
		return memory;
	}
#elif WINDOWS
	void* m_allocate_page_aligned_memory(size_t length) {
		void* memory_ptr;
		
		if (length % 4096) {
			length = length + 4096 - (length % 4096);
		}
		
		#if DEBUG
			++number_of_allocations;
			number_of_bytes_allocated += length;
			
			output_nullt_string("Allocating a page-aligned block of ");
			putu(length);
			output_nullt_string(" bytes.\n");
		#endif
		
		memory_ptr = _aligned_malloc(length, 4096);
		
		if (unlikely(!memory_ptr)) {
			output_nullt_string("_aligned_malloc() failed\n");
			exit_program();
		}
		
		return memory_ptr;
	}
#endif

#if WINDOWS
	static void m_make_memory_only_executable(void* restrict address, const size_t length) {
		DWORD old_protection_flags;
		BOOL return_value = VirtualProtect(address, length, PAGE_EXECUTE, &old_protection_flags);
		
		if (unlikely(!return_value)) {
			output_nullt_string("VirtualProtect() failed.\n");
			exit_program();
		}
	}
#endif

#if UNIX
	attr_nonnull void m_free_memory_for_code(void* restrict address
	#if USE_MMAP
		, u length
	#endif
	) {
		assert_comparison(address, !=, NULL, void* restrict, void*);
		
		#if USE_MMAP
			#if DEBUG
				++number_of_frees;
				
				if (unlikely(number_of_frees > number_of_allocations)) {
					more_frees_than_allocs_error();
				}
			#endif
			
			if (unlikely(munmap(address, length) == -1)) {
				output_nullt_string("munmap() failed: ");
				output_errno_string();
				output_newline();
			}
		#else
			m_free(address);
		#endif
	}
#elif WINDOWS
	void m_free_memory_for_code(void* restrict address) {
		#if DEBUG
			++number_of_frees;
			
			if (unlikely(number_of_frees > number_of_allocations)) {
				more_frees_than_allocs_error();
			}
		#endif
		
		_aligned_free(address);
	}
#else
	#error "FIXME"
#endif

#if DEBUG
	char* m_alloc(const size_t size) {
		++number_of_allocations;
		number_of_bytes_allocated += size;
		
		if (unlikely(number_of_frees > number_of_allocations)) {
			more_frees_than_allocs_error();
		}
		
		#if VERBOSE_ALLOCATIONS_AND_FREES
			printf("malloc(%lu)\n", size);
		#endif
		
		return (char*)malloc(size);
	}
	
	char* m_zero_initialized_alloc(const size_t size) {
		++number_of_allocations;
		number_of_bytes_allocated += size;
		
		if (unlikely(number_of_frees > number_of_allocations)) {
			more_frees_than_allocs_error();
		}
		
		#if VERBOSE_ALLOCATIONS_AND_FREES
			printf("calloc(1, %lu)\n", size);
		#endif
		
		return (char*)calloc(1, size);
	}
	
	char* m_realloc(void* cr ptr, const size_t size) {
		++number_of_frees;
		++number_of_allocations;
		number_of_bytes_allocated += size;
		
		if (unlikely(number_of_frees > number_of_allocations)) {
			more_frees_than_allocs_error();
		}
		
		#if VERBOSE_ALLOCATIONS_AND_FREES
			printf("realloc(ptr, %lu)\n", size);
		#endif
		
		return (char*)realloc(ptr, size);
	}
	
	void m_free(void* cr ptr) {
		assert_comparison(ptr, !=, NULL, void*, void*);
		
		++number_of_frees;
		
		if (unlikely(number_of_frees > number_of_allocations)) {
			more_frees_than_allocs_error();
		}
		
		#if VERBOSE_ALLOCATIONS_AND_FREES
			puts("free(ptr)");
		#endif
		
		free(ptr);
	}

	void show_memory_usage(void) {
		++number_of_frees;
	
		if (number_of_frees < number_of_allocations) {
			putu(number_of_allocations - number_of_frees);
			output_nullt_string(" block");
			if (number_of_allocations - number_of_frees == 1) {
				output_nullt_string(" was");
			} else {
				output_nullt_string("s were");
			}
			output_nullt_string(" not freed.\n");
		} else if (number_of_frees > number_of_allocations) {
			more_frees_than_allocs_error();
			output_newline();
		} else {
			output_nullt_string("All blocks were freed.");
			output_newline();
		}
	
		output_nullt_string("Heap usage: ");
		putu(number_of_allocations);
		output_nullt_string(" alloc");
	
		if (number_of_allocations != 1) {
			output_char('s');
		}
	
		output_nullt_string(", ");
		putu(number_of_frees);
		output_nullt_string(" free");
	
		if (likely(number_of_frees != 1)) {
			output_char('s');
		}
	
		output_nullt_string(", ");
		putu(number_of_bytes_allocated);
		output_nullt_string(" byte");
		if (likely(number_of_bytes_allocated != 1)) {
			output_char('s');
		}
	
		output_nullt_string(" allocated\n");
	}
#endif

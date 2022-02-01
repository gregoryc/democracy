#include "errno.h"
#define OUTPUT_FUNCTIONS_MODULE
#include "headers.h"

/* There's much more output in DEBUG mode, so set INITIAL_OUTPUT_BUFFER_SIZE appropriately. */
#if DEBUG
	#define INITIAL_OUTPUT_BUFFER_SIZE  5000
#else
	#define INITIAL_OUTPUT_BUFFER_SIZE  200
#endif

static char* output_buffer = NULL;
static u64 output_buffer_offset = 0;
static u64 output_buffer_allocated_length = INITIAL_OUTPUT_BUFFER_SIZE;

void setup_output_buffer(void) {
	output_buffer = m_alloc(INITIAL_OUTPUT_BUFFER_SIZE);
	
	if (!output_buffer) {
		fputs(
			"Unable to allocate memory for output buffer.\n"
			"Since this done in the very beginning of the program, some other process(es) on the system must be hogging all the memory.\n",
			stderr
		);
		exit(1);
	}
}

/*
This function tries to reallocate with double the necessary amount of memory.
If that fails it then tries 1.5 times the necessary amount, then 1.25x,
then 1.1x, then 1.05x, then 1.0x, and if that fails it will output and error
message and abort.

Doing this prevents an error if the neccessary amount is some very huge amount of memory (such as one
gigabyte). If the neccessary amount were one gigabyte, it would try to allocate two gigabytes, and on a
system with only two gigabytes of RAM and no swap space it would fail, since it would be requesting ALL
the memory on the system. If this were the case, it would try smaller and smaller amounts of memory
(1.5x, 1.25x, 1.1x, 1.05x and 1.0x) until it's successful, and if it isn't it would print an error message
and abort.
*/
static void reallocate_output_buffer(u64 length) {
	if (output_buffer_offset + length < output_buffer_allocated_length) return;
	
	/* 1 is added because output_buffer_offset is an offset, not a length counter variable. */ 
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 2;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((u64)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 1.5;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((u64)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 1.25;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((u64)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 1.1;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((u64)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 1.05;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((u64)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length);
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((u64)output_buffer)) return;
	
	exit(1);
}

void output_string(const char* const restrict string, const u64 length) {
	assert_comparison(string, !=, NULL);
	assert_comparison(length, !=, 0);
	assert_comparison(output_buffer, !=, NULL);
	
	reallocate_output_buffer(length);
	memcpy(output_buffer + output_buffer_offset, string, length);
	output_buffer_offset += length;
}

void output_char(const int char_) {
	assert_comparison(output_buffer, !=, NULL);
	
	reallocate_output_buffer(1);
	output_buffer[output_buffer_offset++] = char_;
}

void output_newline(void) {
	assert_comparison(output_buffer, !=, NULL);
	
	output_char('\n');
}

void output_nullt_string(const char* const restrict string) {
	u64 length_of_string;
	
	assert_comparison(string, !=, NULL);
	assert_comparison(output_buffer, !=, NULL);
	
	length_of_string = str_len(string);
	assert_comparison(length_of_string, !=, 0);
	reallocate_output_buffer(length_of_string);
	memcpy(output_buffer + output_buffer_offset, string, length_of_string);
	output_buffer_offset += length_of_string;	
}

void output_u64(const u64 number) {
	char buffer[27];
	
	assert_comparison(output_buffer, !=, NULL);
	
	add_commas_to_u64(number, buffer);
	output_nullt_string(buffer);
}

void output_u64_without_commas(const u64 number) {
	char buffer[21];
	
	assert_comparison(output_buffer, !=, NULL);
	
	u64_to_string(number, buffer, 10);
	output_nullt_string(buffer);
}

void output_errno_string(void) {
	output_nullt_string(strerror(errno));
}

void output_f64(const f64 number) {
	char buffer[
		10 /* max number of digits in string */ +
		1 /* for possible decimal point */ +
		5 /* for possible e+DDD, or e-DDD, where the D's are digits */ +
		1 /* for null terminator */
	];
	
	assert_comparison(output_buffer, !=, NULL);
	
	sprintf(buffer, "%.10g", number);
	output_nullt_string(buffer);
}

void output_everything(void) {
	fwrite(output_buffer, output_buffer_offset, 1, stdout);
	fclose(stdout);
	
	free(output_buffer);
	
	#if DEBUG
		output_buffer = NULL;
	#endif
}

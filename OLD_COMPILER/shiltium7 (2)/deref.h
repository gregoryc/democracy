#if DEBUG
	#if __GNUC__
		attr_noreturn static inline void* __null_pointer_deref_error_message(
			const int line,
			const char* const restrict file,
			const char* const restrict function
		) {
			output_nullt_string("\nError in file ");
			output_nullt_string(file);
			output_nullt_string(", in ");
			output_nullt_string(function);
			output_nullt_string("(), on line ");
			output_u64(line);
			output_nullt_string(": dereferencing NULL pointer\n");
			exit(1);
		}
		
		#define DEREF(x) (((x) ? (*(x)) : (__null_pointer_deref_error_message(__LINE__, __FILE__, __func__), *((__typeof__(x))NULL))))
		#define DEREF_ASSIGN(x) (void)((!(x)) && __null_pointer_deref_error_message(__LINE__, __FILE__, __func__)), (*x)
		#define ARRAY_INDEX(x, i) DEREF((x) + (i))
		#define ARRAY_INDEX_ASSIGN(x, i) (void)((!(x)) && __null_pointer_deref_error_message(__LINE__, __FILE__, __func__)), (x)[i]
		#define ARROW(x, y) DEREF(x).y
		#define ARROW_ASSIGN(x, y) DEREF_ASSIGN(x).y
	#else
		#define __FASTER_DEREF  1
	#endif
#else
	#define __FASTER_DEREF  1
#endif

#ifdef __FASTER_DEREF
	#define DEREF(x) (*(x))
	#define DEREF_ASSIGN(x) (*(x))
	#define ARRAY_INDEX(x, i) (x)[i]
	#define ARRAY_INDEX_ASSIGN(x, i) (x)[i]
	#define ARROW(x, y) (x)->y
	#define ARROW_ASSIGN(x, y) (x)->y
#endif

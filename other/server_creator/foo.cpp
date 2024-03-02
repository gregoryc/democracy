#ifndef foo_macro
		#define foo_macro
		#undef foo
		#undef foo_otherwise
		#ifndef nop_func_macro
		#define nop_func_macro
		static double* nop_func(...) { return nullptr; }
		#endif
		#define foo_otherwise nop_func

    #define function static inline void*
    #define var auto
    #define let auto
    #define const const auto
    function foo(Array_sub<BasicObject>  array, size_t i) {double *  rvalue
        
        ;
        const max = array.size();
        while (i < max) {
               BasicObject element = array[i];
                  
                const condition = true;
                if (condition) {
                		using namespace std;
                		
                		//{one.slice(one.length - 1)
                        rvalue = *(double**)(size_t)(size_t*)&(std::cout << "Array is " << (string)array <<endl);
                } else {
                        rvalue = foo_otherwise(&(element), 0);
   }  ++i; } 
return rvalue;
}
#endif

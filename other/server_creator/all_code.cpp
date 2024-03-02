
#include <cstddef>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <map>

//namespace {
enum {
	INT,
	DOUBLE,
	STRING,
	VECTOR,
	MAP,

	NEEDS_FREEING,
	NEEDS_DELETING,
	NEEDS_DELETING_ARRAY
};



class BasicObject {
public:
	operator const std::string() const;
	
	
	const BasicObject& print() const;
	
	
	bool operator < (BasicObject a) const {
		return (const std::string)(*this) < (const std::string)(a);
	};


public:
	double type;
	double value;
};


class Object_That_Destructs : public BasicObject {
public:

	~Object_That_Destructs() {
		union A {
			char* c;
			double d;

		};

		A a;
		a.d =value;

		
			switch ((size_t)type) {
				case NEEDS_FREEING:
//					std::cout << "Freeing " << (std::string)(*this) << "\n";
					free(a.c);
					break;
				case NEEDS_DELETING_ARRAY:
					delete a.c;
					break;
				case NEEDS_DELETING:
					delete [] a.c;
					break;
			}
	};
	

};


typedef Object_That_Destructs Object;


template <class A>
class Array_sub : public std::vector<A>
{
public:
	Array_sub(std::initializer_list<A> a) : std::vector<A>{a}
	{}

		inline void push(const BasicObject value) {
		std::vector<A>::emplace_back(

		*(A*)(&value)
		);
	}

	


	inline Array_sub<A>& operator<< (BasicObject v) {
		push(v);
		return *this;
	}

	inline Array_sub<A>& operator<< (Object v) {
			push(v);
			return *this;
		}

	inline const auto surround(const char* first, const char* second)   const {

		Array_sub<BasicObject> b{};
		//std::cout << "Start of loops\n";
		for (size_t i = 0; i < this->size(); ++i) {
			std::string s{first};
			s+= (std::string)((*this)[i]);
			s += second;
			
			b.emplace_back(construct_needs_freeing(strdup(s.c_str())));
//			std::cout << "Loop number " << i << std::endl;
		}
//		std::cout << "End of loops\n";
		return b;
	}

	inline auto js() {
		return surround("<script type='text/javascript'>", "</script>");
	}

	inline const    auto construct_needs_freeing(const char* const a)   const   {
		union {
			const char* c;
			double d;
		} u;

		u.c = a;

		return *new BasicObject{NEEDS_FREEING, u.d};
	}

	

	
	operator const std::string() const {
		return to_s();
	}

	inline auto append(const char* arg1) const {
		return surround(nullptr, arg1);
	}

	
	inline auto prepend(const char* arg1)   const {
		return surround(nullptr, arg1);
	}

	inline auto quote()      const {
		return surround("'", "'");
	}

	inline auto join(const char* delim)        const {
		std::string result;
		const auto length=this->size();
		for (size_t i = 0; i < length; ++i) {
			result += (*this)[i];
			if (i==length-1) {
			} else {
				result += delim;
			}
		}

		return result;
	}

	inline auto html(const char* title) const {
		std::string site_content;
		site_content += "Content-type: text/html\r\n\r\n<!DOCTYPE html><html><head><title>";
		site_content += title;
		site_content += "</title></head>";
		site_content += "<body>";
		//this->surround("<p>", "</p>");
		site_content += this->surround("<p>", "</p>");
		site_content += "</body></html>";
		return site_content;
	}
	
	inline auto p() const {
		return surround("<p>", "</p>");
	}

	inline auto div()  const {
		return surround("<div>", "</div>");
	}

	inline auto nls()    const      {
		return surround(nullptr, "\n");
	}


	
	const std::string to_s()   const {
		std::ostringstream str;
		str << '[';
	for (size_t  i = 0; i < this->size(); ++i) {
		str << (std::string)((*this)[i]);

		if (i == this->size() - 1) {
		} else {
			str << ", ";
		}
	}
		str << ']';



return str.str();

}
};


//typedef Array_sub<BasicObject> Array;


	union Object_v {
		double d;
		const void* v;
		long long int long_long;

		const std::map<BasicObject, BasicObject>* map;
//const char* string;
		const Array_sub<BasicObject>* array; /*/const ch
const cha*/
		const char* string;
	};








BasicObject::operator const std::string()   const  {
	Object_v val_;

	val_.d = this->value;

	switch ((size_t)this->type) {
		case STRING:
			return val_.string;
		case NEEDS_DELETING:
			return "[needs_deleting]";
		case NEEDS_FREEING:return val_.string;
//			return std::string(val_.string) + ";
		case NEEDS_DELETING_ARRAY: return "[needs_deleting_array]";
		case INT:
			return std::to_string((long long int)val_.long_long                       );
		case DOUBLE:
			return std::to_string(val_.d);
		case VECTOR:
			return val_.array->to_s();
		case MAP:
				std::string map_str = "{";
//			std::map<A,

			//auto& map = val_->map;
			for (const auto& i :  *val_.map) {
				map_str += (std::string)(i.first);
				map_str += ": ";
				
				map_str += (std::string)(i.second);

				map_str += ", ";
			}
//			map_str[map_str.size() = 
			if (map_str.size  ()          ) {
				map_str[map_str.size() - 2] = '}';
				map_str.resize(map_str.size() - 1);
			}

//			std::cout << map_str;

			return map_str;
	}

	return "Error string";
};







const BasicObject&
BasicObject::print() const {
	std::cout << (std::string)(*this) << "<br/>";
	return *this;
}
auto construct(const long long int a) { union Object_v un; un.long_long = a; return BasicObject {0,   un.d};}
auto construct(const std::map<BasicObject, BasicObject> && a) {Object_v un; un.map = &a; return BasicObject{4, un.d}; }
auto construct(Array_sub<BasicObject> && a ) {Object_v  q; q.array = &a; return BasicObject{3,       q.d}; }
auto operator ""_c(unsigned long long a) {
	return construct(a);
}

auto construct(const double a) {
	return BasicObject{1,        a};

}


auto construct(const char* a) {
	union {
		double d;
		const void* v; } b;
	b.v = a;

	return BasicObject{2, b.d};
}




#include "fcgio.h"
using namespace std;
#define endl "<br />"  
static inline void* e1686514597183_main ();
static int mmain();

int
main_
(
	int argc, char** argv, char** envp)
{
	(void)envp;
	Array_sub<Object> args{};
	for (int i = 0; i < argc; ++i)
		args << construct(argv[i]);
	//auto content = (string)args.html("Title"); //cout << c
	auto content=std::string{};
	//cout << Array_sub<Object>{(Object)construct(1ll)}.html("Hello");
	//cout << content;

	
	const auto server=0;

	if (server) {
    // Backup the stdio streambufs
    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();

    FCGX_Request request;

    FCGX_Init();
    /*
    *
    *
    *
    * The above function seems to memory leak 1 block of 768 bytes. It never deletes it at exit.
    *
    *
    *
    */
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);

        cout << content;

//             system("echo 5 > /root/six");
             mmain();
             cout << "Where?";
//             system("echo 6");

        // Note: the fcgi_streambuf destructor will auto flush
    }

    // restore stdio streambufs
    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);
    }

    e1686514597183_main();

    return 0;
}


static int mmain()
{
    std::string command("ls * 2>&1");

    char buffer[4096];
    std::string result;

//    std::cout << "Opening reading pipe"
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Couldn't start command." << endl;
        return 0;
    }
    while (fgets(buffer, sizeof(buffer),          pipe) != NULL) {
        //std::cout << "Reading..." << std::endl;
        result += buffer;
    }
    auto returnCode = pclose(pipe);

    std::cout << result << endl;
    std::cout << returnCode << endl;

//    e1686514597183_main();

	e1686514597183_main ();
    return 0;
}
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

  function e1686514597183_main() {
  	return 
foo(
 Array_sub<BasicObject> {construct(1ll), construct(
		                  std::map<    BasicObject, BasicObject  >{std::pair<  BasicObject ,  BasicObject >(construct("foo"),construct("bar"))})  ,   /* end of map */},
0
);
  }


int main(int argc, char ** argv  , char ** envp) {   main_(     argc, argv, envp); }
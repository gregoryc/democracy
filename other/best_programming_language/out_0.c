
/*    Emerald C */
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
static const char* lastr = NULL;


char* itoa(long val, char* buf) {
	char* p;
    unsigned long a;
    char* b;
    char temp;
    unsigned long u;

    p = buf;

    if (val < 0)
    {
        *p++ = '-';
        val = 0 - val;
    }
    u = (unsigned long)val;

    b = p;

    do {
        a = u %10;
        u /=10;
        *p++ = a + '0';
    } while (u > 0);

//    long len = (long)(p - buf);

    *p-- = 0;
    do {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;
    } while (b < p);

    return buf;
}




static inline char match_chars(const char *restrict expr, const char **restrict pattern) {
  // puts(pattern);
  (*pattern)++;
  //  puts(*pattern);
  const char val = *(expr);
  int res = 0;
  while (**pattern != ']') {
    // putchar(**pattern);

    //    putchar(val)d;

    if (val == **pattern) {
      res = 1;
    }
    ++*pattern;
    // return false;
  }

  ++*pattern;

  return res;
}
static inline void add(char a, const char b, size_t *restrict const i,
                       char *restrict const new_pattern) {
  ++a;

  while (a < b) {
    new_pattern[*i] = a++;
    (*i)++;
  }
}


static inline char match(const char *restrict expr, const char *pattern) {
  {
    // for (int i = 0; i < len; ++i) {
    while (*expr) {
      if (!*pattern) {
        return 1;
      }
      if (*pattern == '[') {
        int res = match_chars(expr, &pattern);
        if (!res) {
          //      	puts;
          return 0;
        }
        ++expr;
        //      continue;
        continue;
      }
      if (*expr == *pattern) {

        ++expr;
        ++pattern;
        continue; // continue;
      }           // else
      //        goto nexr, *pattern);
      //
      return 0;

      //    next:
    }
  };
  return (!(*expr || *pattern));
}

static inline void setup_mini_reg(const char *pattern, char *buf) {
  size_t i = 0, i2 = 0;
  const size_t l = strlen(pattern);

  while (i < l) {
    if (pattern[i] == '-') {
      add(pattern[i - 1], pattern[i + 1], &i2,buf);

      ++i;
      continue;
    }

    (buf)[i2++] = pattern[i++];
  }

  (buf)[i2] = 0;
}

static inline unsigned int reg(const char *restrict expr,
                                const char *restrict const new_pattern) {
  for (; *expr; ++expr) {
    if (match(expr, new_pattern)) { //,  )) {
      return 1;
    } // else {
  }

  return 0;
}

static void* lasts=NULL;
static char* sep="";

static inline void freel(char **ptr) {
if (lastr) free((void*)lastr);
if (lasts)  free((void*)lasts);
lastr=0; lasts = 0;
if (*ptr) free(*ptr);
*ptr = 0;
}

static inline void chomp(char *s) {
    char *ptr;
    if (!s)
        return;   // handle NULL string
    if (!*s)
        return;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = 0;
}



static inline char* strip(char* a) {
	while (isspace(*a)) {
		++a;
	}
	chomp(a);

	return a;
}


static inline int starts_with(const char *restrict string, const char *restrict prefix)
{
    while(*prefix)
    {
        if(*prefix++ != *string++)
            return 0;
    }

    return 1;
}
static inline int ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

static inline const char* join(const char** strings, size_t num_strings, const char* seperator) {
    int input_strings_length = 0;
    for (size_t    i = 0; i < num_strings; i++) {
        input_strings_length += strlen(strings[i]);
    }

    size_t string_length = input_strings_length + num_strings * strlen(seperator);
    char* joined = malloc((string_length + 1) * sizeof(char));
    joined[string_length]=0;

    int array_offset = 0;

    for (size_t string_index = 0; string_index < num_strings; string_index++) {
        if (string_index != 0) {
            strcpy(joined + array_offset, seperator);
            array_offset += strlen(seperator);
        }

        strcpy(joined + array_offset, strings[string_index]);
        array_offset += strlen(strings[string_index]);
    }

    return joined;
}

static inline size_t printint(size_t i) {
	printf("%ld\n",  i); return i;
}

# 1 "./example.c"

# 2 "./example.c"

# 3 "./example.c"
int main() {
       
# 4 "./example.c"
	FILE* f = fopen("/usr/share/dict/words", "r");
# 5 "./example.c"
	char*line=0;ssize_t _len_=0;size_t __size=0;while((_len_=getline(&line,&__size,f))!=-1){
# 6 "./example.c"
		chomp(line);
# 7 "./example.c"
		puts( (lasts=(void*)join( (const char*[]) { "Line ", line, "", }, 3, sep)));
# 8 "./example.c"
		freel(&line);
# 9 "./example.c"
	}
# 10 "./example.c"
	freel(&line);
# 11 "./example.c"
	fclose(f);
# 12 "./example.c"
}





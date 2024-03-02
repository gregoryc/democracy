#!/usr/bin/crystal

require "colorize"

class String
  def bold
    return self
    colorize.bold
  end

  def blue
    return self
    colorize :blue
  end

  def red
    return self
    colorize :red
  end
end

if %w[--features --help -h].includes? (ARGV[0] rescue "-h")
  count = 0
  puts "A better C".blue.bold
  puts "C is a fantastic language. C compiles insanely fast, is the fastest language there is, is very clear, is native to all systems, and is useful for all purposes.".blue.bold

  puts "Some people, e.g., Bjarne Stroustrup, consider that C is `not good enough`, and make languages like C++ or D (or thousands of others)".blue.bold
  puts "This is not necessary.".blue.bold
  puts "There is not reason to use Python instead of C for `simple scripting tasks`".blue.bold

  # puts

  puts "Some functions can be used as methods".blue.bold
  puts "Makes for an excellent `scripting language` (which is really just native C without things making it slow)\n".blue.bold

"
  String Interpolation \"String interplation like this \#{foo} \" Calls the join() function (talked about below to join strings, delim is a static global in each module called `sep`. Make sure you free() the string after. The string is stored in a static global variable called `last`.  So you could do puts(...); free(last)

  ew     *i.ew(\"Foo\"),   a.ew(\"bar\")   | char* ends with another char*?
sw   *i.sw(\"Foo\"),   a.sw(\"bar\")   | char* starts with another char*?
==   Comparison of strs,  \"foo\" ==(\"bar\")
MISC features || autoinclude, foo.gsub or gsub(...),no need for paren (i.chomp, i.strlen), starts_with, ends_with, --wrap, itoa, freel (frees str, sets to NULL, and frees other recent stuff)
strip Returns pointer to string that was stripped, in place
chomp  void function, chomps end of string of spaces, i.chomp  or chomp(i), in place
gsub   Just like ruby, there is a gsub function. The preprocessor detects if you use and adds -lpcre2-8 to the link flags if you use it.  Use $\" for substitutions in argument 3     gsub(a, b, c)

join   Joins an array of strings, to split(), use C's strtok (very fast)
Lightweight regexes are added, almost 15 times as fast as C's built in regex.   They are very simple, they only have capture groups, like [a-z]. To use it, you need to pass a buffer of the size of the expansion of the regex. Foo[a-fA-F] would be Foo[abcdefABCDEF].   That would be stored in a buffer.    From testing, 15x times faster than C's regex POST COMPILATION, if compilation keeps happening, it might be hundres of times faster. Uses static inline functions, NO HEAP MEMORY unless you malloc before().


print Like old python print \"Hello\"(fputs);
printint prints a number and returns the number, can be chained
each_line,    Perlish, different names,  by_line(s) or each_line(s), iterate over all lines, free after
NOTE     You can use open_memstream() on UNIX to easily concatinate strings.  asprintf() is another useful function which is UNIX-centric open_memstream() could be an alternative to std::string It works very well with each_line or by_lines (see above)
".strip.split("\n") do |i|
    next if i.strip == ""
    # puts
    str = String::Builder.build do |str|
      str << (count += 1).to_s.bold.blue
      # str = str.to_s.
      str << " "
      q = i.split
      str << "     "
      str << i.split[0].red
      str << "     "
      str << i.split[1..].join(' ').blue
    end

    # str << i.split[0].red
    # str << i.split[1..].bl
    # puts(str.blue)
    puts str

    #	str = (count += 1).to_s + " " +
    #	puts i
  end

  exit
end

def open_file
  num = 0
  s = File.read(ARGV[0]) rescue (raise "No file")
  s = s.each_line.map do |line|
    num += 1
    if num == 1
      if line.starts_with?("#!")
        line = ""
      end
    end
    "# #{num} #{ARGV[0].dump}\n#{line}\n"
  end.join
  s
end

s = open_file

if ARGV.includes? "--wrap"
  s = "int main(int argc, char** argv) {\n"
end

if ARGV.includes? "--wrap"
  s += "\n}"
end
ARGV.delete("--wrap")

s = "
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
static char* sep=\"\";

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
	printf(\"%ld\\n\",  i); return i;
}

" + s

s = s.gsub(/".+?"/) do |i|
  size = 0
  str = String::Builder.build do |str|
    #  str = St
    scanned = false
    is = [] of String
    ret = i[1..-2].gsub(/#\{.*?\}/) do |q|
      is.push q
      "|"
    end
    if ret == i[1..-2]
      # ret.prepend
      # puts "YES#{i}"
      str << '"'
      str << ret

      str << '"'
      #    end
    else
      str << " (lasts=(void*)join( (const char*[]) { "
      #  if ret == i[2..
      #  if ret =
      ret = ret.split "|"
      #  puts is, ret
      # puts is.size if is !=[]
      # puts ret

      (ret.each do |i|
        q = is.shift || "" rescue ""

        str << '"'
        str << i
        str << '"'

        size += 1
        if ((q && q[2..-2] != nil) rescue false)
          str << ", "; size += 1
          str << q[2..-2]
        end
        str << ", "
      end); str << "}, #{size}, sep))"
    end
  end

  #  str
end

s = s.gsub /([a-zA-Z0-9]+)\.((?:by|each)_line(?:s)?|times)\s*\{\s*\|\s*([^\n]+?)\s*(?:, ([^\n]+?))?\|/m do |i|
  length = $4.tr(", ", "") rescue ""
  length == "" && (length = "_len_")

  var = $3
  if $2 == "times"
    "for (size_t %s = 0; %s < #{$1}; ++%s) {" % ([var] * 3)
  else
    "char*#{var}=0;ssize_t #{length}=0;size_t __size=0;while((#{length}=getline(&#{$3},&__size,#{$1}))!=-1){"
  end
end

if match = (s.match /(?:by|each)_line/)
  # puts "S
  abort "Syntax error: " + match[0]
end

def func(num, a, b, c, d)
  case num
  when 1
    "strcmp(#{a},#{b})==0" # % [a, b]
  when 2
    "starts_with(#{a}, #{b})" # %[a, b]
  when 3
    "ends_with(#{a}, #{b})" # %[a, b]
  when 4
    "gsub(#{a}, #{b})"
  end
end

%w[== sw ew gsub strdup strlen].each_with_index do |i, num|
  if i.match /^[a-zA-Z]/
    i = "." + i
  else
    #		i = "." + i
  end
  # puts i

  s = s.gsub /([a-zA-Z0-9_\"\s]+)#{i}\s*\((.*?)\)/m do |i|
    a = $1 rescue ""
    b = $2 rescue ""
    c = $3 rescue ""
    d = $4 rescue ""
    func(num + 1, a, b, c, d)
  end
end
s = s.gsub /([\)a-zA-Z0-9"]+)\.(strip|chomp|join|strlen|strdup)/, "\\2(\\1)" # do |i|

s = s.gsub /print\s+(.*?)\s*;*\s*\n/, "fputs(\\1,stdout)  ;\n"; ""
flags = ""

if s.match /gsub\s*\(/m
  flags += "-lpcre2-8"

  s = "
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
typedef const char *(*GsubCallbackFunc)(const char*, char**, size_t, const char*);

static inline const char*  _gsub(const char *const string_, const char *const pattern_,
                GsubCallbackFunc callback,
                const char*  data
) {
  // const char* foo(char* a, char* b, callback) {

  char *str;
  size_t size;

  //	FILE* aa = open_memstream(&s&size);
  FILE *aa = open_memstream(&str, &size);

  size_t end;
  size_t old_start_offset;
  pcre2_code *re;
  PCRE2_SPTR pattern; /* PCRE2_SPTR is a pointer to unsigned code units of */
  PCRE2_SPTR subject; /* the appropriate width (in this case, 8 bits). */

  int errornumber;
//  int find_all;
  int rc;

//  uint32_t option_bits;
//  uint32_t namecount;
//  uint32_t name_entry_size;
//  uint32_t newline;

  PCRE2_SIZE erroroffset;
  PCRE2_SIZE *ovector;
  PCRE2_SIZE subject_length;

  pcre2_match_data *match_data;
  pattern = (PCRE2_SPTR)pattern_;
  subject = (PCRE2_SPTR)string_;
  subject_length = (PCRE2_SIZE)strlen((char *)subject);
  // int i=0;
  int i;

  re = pcre2_compile(
      pattern,               /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      0,                     /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

  /* Compilation failed: print the error message and exit. */

  if (re == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
    fprintf(stderr, \"PCRE2 compilation failed at offset %d: %s\\n\", (int)erroroffset,
           buffer);
    return NULL;
  }

  match_data = pcre2_match_data_create_from_pattern(re, NULL);
  size_t start_offset = 0;

  // old_last_offset = 0;
  /* Now run the match. */
  while (start_offset < subject_length) {
    rc = pcre2_match(re,             /* the compiled pattern */
                     subject,        /* the subject string */
                     subject_length, /* the length of the subject */
                     start_offset,   /* start at offset 0 in the subject */
                     0,              /* default options */
                     match_data,     /* block for storing the result */
                     NULL);          /* use default match context */

    /* Matching failed: handle error cases */

    if (rc < 0) {
      switch (rc) {
      case PCRE2_ERROR_NOMATCH:
        //        printf(\"No PCRE2 match\\n\");
        goto end;
      /*
      Handle other special cases if you like
      */
      default:
        printf(\"Matching error %d\\n\", rc);
        goto end;
      }
      pcre2_match_data_free(match_data); /* Release memory used for the match */
      pcre2_code_free(re);               /*   data and the compiled pattern. */
      return NULL;
    }
    // printf(\"A%d\\n\", rc);

    /* Match succeeded. Get a pointer to the output vector, where string offsets
    are stored. */

    ovector = pcre2_get_ovector_pointer(match_data);
    //    printf(\"Match succeeded at offset %d\\n\", (int)ovector[0]);
    // succeeded
    /*************************************************************************
     * We have found the first match within the subject string. If the output *
     * vector wasn't big enough, say so. Then output any substrings that were *
     * captured.                                                              *
     *************************************************************************/

    /* The output vector wasn't big enough. This should not happen, because we
    used pcre2_match_data_create_from_pattern() above. */

    if (rc == 0)
      printf(\"ovector was not big enough for all the captured substrings\\n\");

    /* Since release 10.38 PCRE2 has locked out the use of K in lookaround
    assertions. However, there is an option to re-enable the old behaviour. If
    that is set, it is possible to run patterns such as /(?=.K)/ that use K in
    an assertion to set the start of a match later than its end. In this
    demonstration program, we show how to detect this case, but it shouldn't
    arise because the option is never set. */
    // int
    char *dup = (char *)malloc(subject_length + rc);
    char **ptrs = malloc(sizeof(char *) * (rc - 1));

    size_t i_ = 0;
    for (size_t q = 1; q < (size_t)rc; q += 1) {
      PCRE2_SPTR substring_start = subject + 0 + ovector[2 * q];
      (void)substring_start;
      PCRE2_SIZE substring_length = ovector[2 * q + 1] - ovector[2 * q];

      ptrs[q - 1] = dup + i_; // dup + i_];
      size_t qq = 0;
      //    int I
      for (; qq < substring_length;) {
        //      for (size
        dup[i_] = substring_start[qq];
        ++i_;
        ++qq;
      }
      dup[i_++] = 0;

      //

      // for (int i = 0;i<rc;++i) {
      // puts(\"A+\");
      // puts  (ptrs[q-1]);
      // puts(\"A\");

      //}
      // exit(0);

      // return 0; }
      // exit(0);
    }

    old_start_offset = start_offset;
    for (i = 0; i < 1; i++) {
      PCRE2_SPTR substring_start = subject + 0 + ovector[2 * i];
      PCRE2_SIZE substring_length = ovector[2 * i + 1] - ovector[2 * i];
      //    printf(\"%d %s\\n\", substring_length, substring_start);
      old_start_offset = start_offset;
      start_offset = ovector[0] + substring_length;
      end = ovector[0] - 1;

      if (end == (size_t)-1) {
        end = 0;
      }
      //     printf(\"%d - %d\\n\", old_start_offset, end);
      /*if (end - old	for */

      if (end - old_start_offset)
        for (size_t i = old_start_offset; i <= end; ++i) {
          putc_unlocked(subject[i], aa);
        }

      //      puts(\"\");

      char *buf = malloc(substring_length + 1);
      for (unsigned long i = 0; i < substring_length; ++i) {
        buf[i] = substring_start[i];
      }
      buf[substring_length] = 0;
      //    puts(buf);
      fputs(callback(buf, ptrs, rc - 1, data), aa);
      free(buf);
    }

    free(dup);
    free(ptrs);

    //    free(ptrs);
    // free(
  }
end:
  //  puts(subject[end]);
  //  if (end - old_start_offset)
  for (int i = start_offset;; ++i) {
    if (subject[i]) {
      putc_unlocked(subject[i], aa);
    } else
      break;
  }

  pcre2_match_data_free(match_data); /* Release the memory that was used */
  fclose(aa);
  pcre2_code_free(re); /* for the match data and the pattern. */
 lastr=str;
  // pcre2_match_data* a;

  return str;
}

static inline const char* _replace_str(const char * _, char ** __, size_t ___, const char* str) {
(void)_;
(void)__;
(void)___;
	return   str;
}

static inline const char* gsub_func(const char* a, const char* b, GsubCallbackFunc c, int free_mem) {
	const char* new_ = _gsub(a, b, c, NULL);
	static const char* prev=NULL;

	if (free_mem) {
		if (prev)  {
			free((void*)prev);
		}
	}
	prev = new_ ;

	return new_;
}


static inline const char*  gsub(const char* a, const char* b, const char* c, int free_mem) {

	const char* new_ = _gsub(a, b, _replace_str, c);

		static const char* prev=NULL;
	if (free_mem) {
		if (prev)  {
	free((void*)prev);
	}
	}
	prev = new_ ;

	return   new_;
}   " + s
end

s += "



"
s = s.gsub /main\s*\(.*?\)\s*\{/m do |i|
  i + "\n       "
end
HEADERS = "
/*    Emerald C */
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
static const char* lastr = NULL;

"
s = HEADERS + s

# if ARGV.join.cinl
File.write file = "out_#{0}.c", s
out_path = "./a.out"
if File.exists?("ERR")
  raise "ERR"
end
cmd = ""
if (ENV["COMPILER"] rescue "") == "gcc"
  cmd = "gcc -O3 -fwhole-program " + flags + "      -Werror -Wfatal-errors -Wall -Wextra  -s #{file} -o #{out_path} #{flags} #{ARGV[1..].join(' ')} 2>ERR"
end
if ["tcc"].includes?(ENV["COMPILER"]? || "tcc")
  cmd = "tcc " + flags + "  -Wall -Wextra  #{file} -o #{out_path} #{flags} #{ARGV[1..].join(' ')} 2>ERR"
end
puts cmd
system cmd

err = File.read("ERR").strip
File.delete "ERR"
if err.size > 5
  if res = err.match /:(\d+): error: /
    num = res[1].to_i - 3
    f = File.read(ARGV[0])
    f = f.split("\n")
    txt = " <<< " + err
    txt = txt.red.bold
    txt = txt.to_s
    3.times do |i|
      o = "Line #{num} > #{f[num]}"
      case i
      when 1
        o = o.red.bold.to_s + txt
      else
        o = o.blue
      end
      puts o
      num += 1
    end
  end
end

system out_path

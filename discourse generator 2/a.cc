#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <cctype>
#include <cstdio>
#include <vector>
#include <ctime>
#include <map>
#include <set>


using namespace std;
namespace {

#if __unix__
#include <unistd.h>


#endif

#if __linux__
#include <string_view>
#include <readline.h>


#else
#define string_view string
#endif

#ifdef push
#error push already defined
#else
#define push emplace_back
#endif

constexpr unsigned long int OPTIONS = 3 + 3;

const set<string_view> valid_names{"negemotion", "negproof", "negrationale",
                                   "posemotion", "posproof", "posrationale"};
namespace Modes {
bool html = false, verbose = false, tty = true;

};
#define MDASH "\xE2\x80\x94"
const auto EXT = ".valid";
#define issep isspace

#define CSS "color: blue;"
#define DOTS "........"
#define COLOR_RESET "\e[0m"
#define BGRN "\e[1;32m"
#define RED "\e[0;31m"
#define OUTPUT_COLOR BGRN
constexpr const char *const HELP = RED
    "Mediocre `filter software' exists to detect bad discourse "
    "so far (in this program and definitely in the world). People must force "
    "people to provide good rationales, emotions and reasons for things. The "
    "powers that be must be held accountable, for not "
    "communicating.\nNow... Generate sentence for use in any "
    "context, e.g., Local Politics, International Politics, International "
    "Diplomacy, Mainstream Academia, Protest Documents, Twitter, Business "
    "Correspondence, and "
    "others.\nForce big businesses, schools and companies to use this "
    "software. Force company policy to employ this software (BSD "
    "licensed, cross-platform, C++, bug-free, robust, no memory "
    "leaks, no security vulnerabilities, C++ STL, small "
    "codebase, easy to understand.). Run tests first. Non-AI, "
    "no-nonsense. Use `html' flag to generate HTML.\n\n" COLOR_RESET;

enum Err {
  ZEROTH,
  CANT_OPEN_FILE,
  BAD_WORD,
  INCOMPLETE,
  ALREADY_HAS_KEY,
  WRONG_SIZE,
  INVALID_NAME,
  LAST_ERROR
};

string_view ERRORS[]{"Can't open filesystem file",
                     "Bad word in the description",
                     "Incomplete",
                     "Already has key",
                     "Wrong size",
                     "Invalid name"};

set<string> bad_words{"because", "since"};

vector<string_view> negative{"disagree with", "don't agree with",
                             "do not agree with"},
    positive{"agree with", "do agree with"},
    neutral{"I think we can all agree", "Everyone knows", "Everybody knows",
            "We all know"},
    proof{"evidenced by",     "shown by",      "indicated by",
          "made apparent by", "made clear by", "made plain by",
          "demonstrated by",  "made plain by", "becomes observable by"},
    rationale{"because",
              "since",
              "; the reason being that",
              "due to the fact that",
              "inasmuch as",
              "; for the reason that"},
    howevers{"one interesting point though",
             "however",
             "though",
             "although",
             "that being said",
             "with that said",
             "on the other hand though"

    };

int error(size_t line, Err num, const string &msg) {
  using namespace Modes;
  string str;
  if (tty) {
    str += RED;
  }
  assert(num < LAST_ERROR);
  str += "On line ";
  str += to_string(line);
  str += ". ";
  str += ERRORS[num];
  str += " (error code ";
  str += to_string(num);
  str += "). ";
  str += msg;
  str += ". Cannot proceed.\nAborting.\n";
  if (tty)
    str += COLOR_RESET;

  fputs(str.c_str(), stderr);

  return 1;
}

void open_file_o(const size_t line_num, const char *const file, ofstream *str) {
  str->open(file, ifstream::out);
  if (str->is_open()) {
  } else {
    error(line_num, CANT_OPEN_FILE, string("Cant open `") + file + "'");
  }
}

void eval(const size_t, map<string, vector<string>> parts_set, string &out) {
  for (auto &i : vector<decltype(&negative)>{&negative, &positive, &proof,
                                             &rationale, &howevers}) {

    random_shuffle(i->begin(), i->end());
  }
  string &&however = string(howevers[0]);
  however[0] = toupper(however[0]);
  array<const void *, OPTIONS> neg{"negemotion", "negrationale", "negproof",
                                   &rationale,   &negative,      &proof};
  random_shuffle(neg.begin(), next(next(neg.begin())));

  array<const void *, OPTIONS> pos{"posemotion", "negrationale", "posproof",
                                   &rationale,   &positive,      &proof};
  random_shuffle(pos.begin(), next(next(pos.begin())));

  array<decltype(neg), 2> neg_pos{neg, pos};

  random_shuffle(neg_pos.begin(), neg_pos.end());

  bool first = true;

  for (auto keys : neg_pos) {
    auto back_iter = keys.end();
    vector<string_view> *proof = *(vector<string_view> **)(--back_iter);
    vector<string_view> *emotion = *(vector<string_view> **)(--back_iter);
    vector<string_view> *rationale = *(vector<string_view> **)(--back_iter);
    auto f = keys.begin();

    auto s = [&]() mutable -> void { out.resize(out.size() - 1); };
    const auto last = [&]() -> char { return out[out.size() - 1]; };

    while (f != back_iter) {
      const char *val = *((const char **)f);
      vector<string> &values = parts_set[val];
      const char *const last_part = (const char *)(*f) + 3;

      if (strcmp(last_part, "emotion") == 0) {
        static vector<string_view> list{"situation", "fact", "condition",
                                        "circumstance", "current reality that"};

        random_shuffle(list.begin(), list.end());
        if (out.size() && last() == ' ') {
          s();
        }
        if (out.size() && last() == ',') {
          s();
        }
        out += ", I ";
        out += emotion[0][0];
        out += " the ";
        out += list[0];
      } else if (strcmp(last_part, "proof") == 0) {
        if (out.size())
          s();
        out += MDASH "all of which is ";
        out += proof[0][0];

      } else if (strcmp(last_part, "rationale") == 0) {
        const auto rat = rationale[0][0];
        if (rat[0] == ';') {
          if (out.size())
            s();
        }
        if (out.size() && last() == ',')
          s();
        out += rat;
      }

      string ac{' '};
      for (size_t i = 1; i < values.size(); ++i)
        ac += values[i] + ' ';

      out += ac;

      ++f;
    }
    if (first) {
      if (out.size())
        s();

      out += ". ";
      out += however;
      out += ", ";
      first = 0;
    }
  }
  if (out.size()) {
    out[out.size() - 1] = '.';
  }
  out += '\n';

  char *cs = (char *)out.c_str();
  if (*cs) {
    while (*cs == ',' || *cs == ';' || *cs == ' ') {
      ++cs;
    };
    *cs = toupper(*cs);
  }
  using namespace Modes;
  string new_str;
  if (html) {
    new_str += ("<div style='" CSS "'>");
    ;
  }
  new_str = cs;
  if (html)
    new_str += "</div>";
  out = new_str;

  if (tty) {
    fprintf(stdout, "%s%s%s", OUTPUT_COLOR, out.c_str(), COLOR_RESET);
  } else {
    fputs(out.c_str(), stderr);
  }
}

inline bool part_invalid(const string &part) {
  return !(valid_names.find(part) != valid_names.end());
}

const string

get_user_input_and_write(size_t line_num) {
  string out_file;
  out_file += to_string(rand());
  out_file += EXT;

  ofstream file_o;

  open_file_o(line_num, out_file.c_str(), &file_o);
  for (auto i : valid_names) {
    string str = BGRN "Enter ";

    str += i;
    str += ">>>  " COLOR_RESET;
#if __linux__

    char *line;
    int break_ = false;

    for (;; free(line)) {
      line = readline(str.c_str());
      char *ptr = line;
      while (*ptr) {
        if (!isspace(*line)) {
          break_ = true;
          break;
        }
      }
      if (break_)
        break;

      ++ptr;
    }
#else
    string line;
    for (;;) {

      fputs(str.c_str(), stdout);
      getline(cin, line);

      bool only_space = true;
      for (auto i : line) {
        if (!isspace(i)) {
          only_space = false;
          break;
        }
      }
      if (!only_space)
        break;
    }

#endif
    string &&a = string(i);
    a += ' ';
    a += line;
    a += "\n";
#if __linux__
    free(line);
#endif

    file_o.write(a.c_str(), a.size());
  }
  file_o.close();
  return out_file;
}
} 

inline void open_file_i(const size_t line_num, const char *const file,
                        ifstream *str) {
  str->open(file, ifstream::in);
  if (str->is_open()) {

  } else {
    error(line_num, CANT_OPEN_FILE, string("Cant open `") + file + "'");
  }
}

int main(int argc, char **argv) {
  unsigned long line_num = 0, file_num = 1;

#if __unix__
  Modes::tty = isatty(1);
#else
#endif

  srand((unsigned int)time(0));
  const auto valid_names_size = valid_names.size();

  string line;
  ifstream ifile;
  ofstream out_file_of_data("out.good.txt", ofstream::out);
  const bool argc_1 = argc == 1;
  for (;; ++file_num, --argc, ++argv) {

    if (argc_1) {
      const string &&out_file = get_user_input_and_write(line_num);
      open_file_i(line_num, out_file.c_str(), &ifile);

    } else if (strlen(argv[1]) > 0 && argv[1][0] == '-') {

      bool cont = false;
      auto arg = argv[1];

      static vector<const char *> options;

      auto c = [&](const char *str, const char *str2 = nullptr) -> bool {
        cont = !strcasecmp(arg, str) || (str2 && !strcasecmp(arg, str2));
        if (!cont) {
          options.push(str);
          if (str2) {
            options.push(str2);
          }
        }

        return cont;
      };
      using namespace Modes;
      if (c("-v")) {
        verbose = true;

      } else if (c("--html")) {
        html = true;
      } else if (c("--nohtml")) {
        html = false;
      } else if (c("--nocolor")) {
        tty = false;
      } else if (c("--color")) {
        tty = true;
      } else if (c("--help", "-h")) {
        map<const char *, const char *> vec{{"--html", "Turn on html mode"},
                                            {"--nohtml", "Turn off html mode"},
                                            {"--color", "Turn on color"},
                                            {"--nocolor", "Turn off color"},
                                            {"-v", "Turn on verbose mode"}};
        puts(RED "HELP" COLOR_RESET);
        for (int i = 0; i < 2; ++i)
          putchar('\n');

        for (const char *const mem : options) {
          const char *const val = vec[mem];
          constexpr const size_t width = 40;
          const size_t len = strlen(mem);
          char dots[len + 1];
          size_t i;
          for (i = 0; i < width - len; ++i) {
            dots[i] = '.';
          }
          dots[i] = 0;
          printf("Option %s%s%s%s%s%s\n", BGRN, mem, COLOR_RESET, dots, " ",
                 val);
        }
        for (int i = 0; i < 80; ++i) {
          putchar('-');
        }
        putchar('\n');

        fputs(HELP, stdout);
        return 1;
      }

      if (cont) {
        if (argc == 2) {
          puts("BREAK1");
          break;
        }
        puts("CON");
        continue;
      }
    } else {
      printf("%d argc\n", argc);
      puts(argv[1]);
      open_file_i(line_num, argv[1], &ifile);
    }

    line.clear();
    map<string, vector<string>> parts_set;

    while (getline(ifile, line)) {
      ++line_num;
      bool no_space = false;

      for (auto iter = line.begin(); iter != line.end(); ++iter) {
        if (isspace(*iter)) {
        } else {
          no_space = true;
          break;
        }
      }
      if (!no_space)
        continue;
      vector<string> parts;

      auto iter = line.begin();
      auto end = line.end();

      for (size_t i = 0; i < 2; ++i) {
        for (; iter != line.end();) {
          string str;
          for (; issep(*iter) && iter != end; ++iter) {
          }

          while (!issep(*iter) && iter != end) {
            str += *iter++;
          }
          if (str.size()) {
            string lower_str;
            for (size_t i = 0; i < str.size(); ++i) {
              lower_str += tolower(str[i]);
            }
            if (bad_words.find(lower_str) != bad_words.end()) {
              return error(line_num, BAD_WORD,
                           string("Bad word '") + str +
                               "'. Cannot be logical.");
            }
            parts.push(str);
          }
        }
      }

      const size_t num_parts = 2;
      if (parts.size() < num_parts) {
        return error(line_num, WRONG_SIZE,
                     "Wrong size of line (" + to_string(parts.size()) +
                         ")\nFormat [reason or feeling or evidence]" DOTS);
      }

      if (part_invalid(parts[0])) {
        return error(line_num, INVALID_NAME,
                     "Invalid name (" + parts[0] +
                         ")\nFormat [reason or feeling or evidence]" DOTS);
      }

      if (parts_set.find(parts[0]) != parts_set.end())
        return error(line_num, ALREADY_HAS_KEY,
                     "Already has key('" + parts[0] + "')");
      parts_set[parts[0]] = parts;
      if (parts_set.size() == valid_names_size) {
        using namespace Modes;

        if (verbose) {
          fprintf(stderr,
                  "File %ld, line %ld. >>> Full set of %ld, on to evaluation. ",
                  file_num, line_num, OPTIONS);
        }

        string out;
        eval(line_num, parts_set, out);
        out_file_of_data.write(out.c_str(), out.size());
        parts_set.clear();
      }
    }

    if (parts_set.size() != 0) {
      return error(line_num, INCOMPLETE,
                   "Incomplete (" + to_string(parts_set.size()) + " values)");
    }

    ifile.close();

    if (argc_1) {
      break;
    }
    if (argc == 2) {
      puts("BREAK");
      break;
    }
  }
  out_file_of_data.close();
  return 0;
}
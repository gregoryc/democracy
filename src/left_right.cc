#define COLOR_RESET "\e[0m"
#define BGRN "\e[1;32m"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

static inline void chomp(char *s, int (*func)(int)) {
  char *ptr;
  if (!s)
    return; // handle NULL string
  if (!*s)
    return; // handle empty string
  for (ptr = s + strlen(s) - 1; (ptr >= s) && func(*ptr); --ptr)
    ;
  ptr[1] = 0;
}

static inline char *strip(char *a) {
  while (isspace(*a)) {
    ++a;
  }
  chomp(a, isspace);

  return a;
}

static int is_punc(int c) { return c == '.' || c == '!' || c == '?'; }

class Done {};
int main() {
  string all_text_0{
      "<html><head>"
      "<title>Politics</title></head><body><center><h1>Rational Viewpoint of "
      "Politics</h1><h2 style=color:blue>BRINGING THE SANITY INTO POLITICS AS "
      "IT SHOULD BE</h2><div style=width:50%>"};
  string all_text, line;
  string quit_str{"done"};
  string null_str;

  try {
    for (;;)
      for (auto party : vector<const char *>{"left", "right"}) {
        char *str;

        for (;;) {
          cout << BGRN "Enter something about the political " << party
               << " that you don't find extreme or radical. (When you're done, "
                  "type done) " COLOR_RESET;
          getline(cin, line);
          str = strip((char *)line.c_str());

          chomp(str, is_punc);
          if (str == quit_str) {
            throw Done{};
          }
          if (*str) {
            break;
          }
        }
        all_text += "<div><b>I believe that the ";
        all_text += party;
        all_text += " is sane, rational and fine due to ";
        all_text += str;
        all_text += '.';
        all_text += "</b></div>\n";
      }
  } catch (Done done) {
  }

  all_text = all_text_0 + all_text;

  all_text += "</body>";

  all_text += "</html>";
  {
    fstream rational_viewpoint("rational_viewpoint.html", ios::out);
    rational_viewpoint << all_text << endl;
  }

  cout << "Written to 'rational_viewpoint.html'" << endl;
  system("google-chrome rational_viewpoint.html");

  return 0;
}
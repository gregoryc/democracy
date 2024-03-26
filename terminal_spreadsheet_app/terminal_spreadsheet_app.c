// Spreadsheet program. Public domain.
// TODO: add numbers, make sure a big spreadsheet works, etc.

// All cells start with ~

#include "./foundationallib.h"
#include <assert.h>
#include <curses.h>
#include <time.h>

typedef struct {
  char *string;
  size_t alloc_size;
  size_t len;
} String;

const int NUM_ROWS = 30;
const int NUM_COLS = 10;
const int MAX_CHAR_SIZE = 10;
#define MAX_CHAR_SIZE_STR "10"
const int BORDER = 2;
const int CELL_WIDTH = MAX_CHAR_SIZE + BORDER; // Adjust the width as needed

void centered_print(WINDOW *win, int y, const char *text) {
  int max_x, max_y;
  getmaxyx(win, max_y, max_x); // Get window dimensions
  (void)max_y;
  int text_length = strlen(text);

  // Calculate the x-coordinate for the start of the text to be centered
  int start_x = (max_x - text_length) / 2;

  // Print the text at the calculated position
  mvwprintw(win, y, start_x, "%s", text);
}

static char *global_status;

static inline void drawSpreadsheet(WINDOW *win, String** data,
                                   int currentRow, int currentColumn) {
  wclear(win);
  box(win, 0, 0);

  const int header_lines = 3;

  for (int i = 0; i < NUM_ROWS; ++i) {
    for (int j = 0; j < NUM_COLS; ++j) {
      const char *ptr = data[i][j].string;

      const int text_in_cell = ptr[0] == '~' && data[i][j].len > 1;
      if (text_in_cell)
        ++ptr;
      if (i == currentRow && j == currentColumn) {
        wattron(win, COLOR_PAIR(3)); // Yellow for current cell
        if (text_in_cell) {
          char *esc = shellescape(ptr);
          if (system("killall xclip 2>/dev/null 1>/dev/null") != 0) {
            /* Couldn't */
          }

          char *cat = concatenate_three_strings("echo -n ", esc,
                                                "|xclip -selection c &");
          free(esc);

          if (system(cat) != 0) {
            /* Couldn't copy */
          } else {
            char *quot = string_to_json(ptr);
            free(global_status);
            global_status =
                concatenate_three_strings("Copied ", quot, " to clipboard.");
            free(quot);
          }

          // append_string_to_file("file.txt", cat);

          free(cat);
        }
      } else {

        if (i == currentRow) {
          wattron(win, COLOR_PAIR(2)); // Red for current row
        } else if (j == currentColumn) {
          wattron(win, COLOR_PAIR(1)); // Green for current column
        }
      }

      mvwprintw(win, i + 1 + header_lines, j * CELL_WIDTH + 1,
                "%." MAX_CHAR_SIZE_STR "s", ptr);
      wattroff(win, COLOR_PAIR(1));
      wattroff(win, COLOR_PAIR(2));
      wattroff(win, COLOR_PAIR(3));
    }
  }

  centered_print(win, 1, global_status);

  wrefresh(win);
}

static void unescape_csv(char **array, size_t lines,
                         String ** data) {
  printf("CSV %zu lines\n\n", lines);
  for (size_t i = 0; i < lines - 1; ++i) {
    size_t num_cells;
    size_t len_of_line = strlen(array[i]);
    if (len_of_line) {
    }

    char **cells = split(array[i], &num_cells, ",", 0, 1);

    for (size_t j = 0; j < num_cells; ++j) {

      size_t len_of_cell = sizeof(char) + strlen(cells[j]);
      if (len_of_cell) {
        // Get rid of comma
        --len_of_cell;
      }
      assert(sizeof(char) == 1);
      data[i][j].alloc_size = len_of_cell + sizeof("");
      data[i][j].string =
          malloc(data[i][j].alloc_size);
      data[i][j].string[0] = '~';

      memcpy(data[i][j].string + sizeof(char), cells[j], len_of_cell);
      data[i][j].string[len_of_cell] = '\0';

      data[i][j].len = len_of_cell;

      printf("\tLine %zu, `%s'\n", i + 1, data[i][j].string);
            
    }

    puts("");
  }

  /*
    char *output;
    size_t new_len;
    size_t num_matches;
    int should_free_after_use;

    // "," => "\,"
    replace_all_with_lens(file_string, file_size, "\\,", 1, ",", 1, &output,
                          &new_len, &should_free_after_use, 0, &num_matches);

    char *output2;
    size_t new_len2;
    size_t num_matches2;
    int should_free_after_use2;
    // "\" => "\\"
    replace_all_with_lens(output, new_len, "\\\\", 1, "\\", 1, &output2,
                          &new_len2, &should_free_after_use2, 0, &num_matches2);

    if (should_free_after_use) {
      free(output);
    }
    return output2;*/
}

void error(const char *str) {
  puts(str);
  exit(1);
}

static void setup_curses(void) {
  initscr();
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  cbreak();
  noecho();
  curs_set(0);
}

int main(int argc, char **argv) {
  String ** data = calloc(NUM_ROWS*NUM_COLS, sizeof(String));
  for (int i = 0; i < argc; ++i) {
  	puts(argv[i]);
  }
  print_string_array(argv, argc);puts(" <<");

  if (argc > 1) {
    char *load_file = argv[1];
  print_string_array(argv, argc);puts(" <<2");
    size_t lines;
  print_string_array(argv, argc);puts(" <<3");
    char **array = read_file_into_array(load_file, "\n", &lines);
  print_string_array(argv, argc);puts(" <<4");  
    printf("%d\n", argc);
    print_string_array(array, lines);

    unescape_csv(array, lines, data);
    printf("%d\n", argc);
    print_string_array(argv, argc);puts(" <<5");
      free_string_array(array, lines);
  print_string_array(argv, argc);puts(" <<6");

    }
  print_string_array(argv, argc);puts(" <<1");

  global_status =
      strdup("SPREADSHEET THAT IS 2-DIMENSION FROM TERMINAL THAT AUTO "
             "COPIES TEXT - FUCK YEAH!!");

  setup_curses();


  WINDOW *spreadsheetWin =
      newwin(NUM_ROWS + 2, NUM_COLS * CELL_WIDTH + 2, 1, 1);
  keypad(spreadsheetWin, true);

  for (int i = 0; i < NUM_ROWS; ++i) {
    for (int j = 0; j < NUM_COLS; ++j) {
      printf("`%s'\n", (data[i][j].string));
      // printf("%d %d\n",       i, j);
      if (data[i][j].len <= 1) {
        data[i][j].string = (char *)malloc(2);
        data[i][j].string[0] = '~';
        data[i][j].string[1] = '\0';
        data[i][j].alloc_size = 2;
        data[i][j].len = 1;
      }
    }
  }

  int currentRow = 0;
  int currentColumn = 0;
  int ch;
  drawSpreadsheet(spreadsheetWin, data, currentRow, currentColumn);
  while (!((ch = wgetch(spreadsheetWin)) == 'q' && currentRow == 0 &&
           currentColumn == 0)) {
    //    printf("`%c'\n", ch);
    switch (ch) {
    case KEY_LEFT:
      currentColumn = (currentColumn - 1 + NUM_COLS) % NUM_COLS;
      break;
    case KEY_RIGHT:
      currentColumn = (currentColumn + 1) % NUM_COLS;
      break;
    case KEY_UP:
      currentRow = (currentRow - 1 + NUM_ROWS) % NUM_ROWS;
      break;
    case KEY_DOWN:
      currentRow = (currentRow + 1) % NUM_ROWS;
      break;
    default:
      if (ch == KEY_BACKSPACE &&
          (data[currentRow][currentColumn].len>1)) { // Handle backspace
        data[currentRow][currentColumn].string[data[currentRow][currentColumn].len - 1] = 0;
        --data[currentRow][currentColumn].len;
      } else if (ch >= 32 && ch <= 126) { // Accept printable ASCII characters
        String *str = &data[currentRow][currentColumn];
        append_string_to_string(&str->string, &str->len, &str->alloc_size,
                                (const char *)&ch, 1);
      }

      break;
    }

    drawSpreadsheet(spreadsheetWin, data, currentRow, currentColumn);
  }
  //  ex();
  endwin();
  free(global_status);

  const size_t alloc_size = NUM_COLS * NUM_ROWS;

  String str = {
      .string = (char *)malloc(alloc_size), .alloc_size = alloc_size, .len = 0};
  for (int i = 0; i < NUM_ROWS; ++i) {
    for (int j = 0; j < NUM_COLS; ++j) {
      String *cell = &data[i][j];
      char *cell_str = cell->string;
      size_t cell_len = cell->len;

      char *output;
      size_t new_len;
      size_t num_matches;
      int should_free_after_use;

      // "\" => "\\"
      replace_all_with_lens(cell_str, cell_len, "\\", 1, "\\\\", 1, &output,
                            &new_len, &should_free_after_use, 0, &num_matches);

      char *output2;
      size_t new_len2;
      size_t num_matches2;
      int should_free_after_use2;

      // "," => "\,"
      replace_all_with_lens(output, new_len, ",", 1, "\\,", 1, &output2,
                            &new_len2, &should_free_after_use2, 0,
                            &num_matches2);
      if (should_free_after_use) {
        free(output);
      }

      char *output3;
      size_t new_len3;
      size_t num_matches3;
      int should_free_after_use3;
      // "\n" => "\\\n"
      replace_all_with_lens(output2, new_len2, "\n", 1, "\\\n", 1, &output3,
                            &new_len3, &should_free_after_use3, 0,
                            &num_matches3);
      if (should_free_after_use2) {
        free(output2);
      }

      if (new_len != 1) /* ~ */ {
        append_string_to_string(&str.string, &str.len, &str.alloc_size,
                                output + 1, new_len - 1);
      }
      append_string_to_string(&str.string, &str.len, &str.alloc_size, ",", 1);
      free(cell->string);

      if (should_free_after_use3) {
        free(output3);
      }
    }

    // Embedded newlines become slash newline, these newlines are for the
    // rows.
    append_string_to_string(&str.string, &str.len, &str.alloc_size, "\n", 1);
  }

  time_t tim = time(NULL);
  char number[FOUNDATIONAL_LIB_SIZE_STRING_OF_NUMBER_SIZE_PLUS_ZERO_TERMINATOR];

  utoa(tim, number);


  char *file = (argc > 1) ? argv[1]:concatenate_three_strings("output", number, ".csv");
  puts(file);
  write_to_file_with_mode(file, str.string, str.len, "w");
  write_to_file_with_mode("/dev/stdout", str.string, str.len, "w");
  printf("\n\nWrote to file `%s'\n", file);
  puts("RETURN 0");
  free(file);

  return 0;
}

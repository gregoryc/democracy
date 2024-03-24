// Spreadsheet program. Public domain.
// TODO: Save the array, add numbers, make sure a big spreadsheet works, etc.
#include "./foundationallib.h"
#include <curses.h>

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

static inline void drawSpreadsheet(WINDOW *win, String data[NUM_ROWS][NUM_COLS],
                                   int currentRow, int currentColumn) {
  wclear(win);
  box(win, 0, 0);

  const int header_lines = 3;

  centered_print(win, 1,
                 "SPREADSHEET THAT IS 2-DIMENSION FROM TERMINAL THAT AUTO "
                 "COPIES TEXT - FUCK YEAH!!");
  for (int i = 0; i < NUM_ROWS; ++i) {
    for (int j = 0; j < NUM_COLS; ++j) {
      const char *ptr = data[i][j].string;
      if (ptr[0] == '~' && data[i][j].len > 1) {
        ++ptr;
      }
      if (i == currentRow && j == currentColumn) {
        wattron(win, COLOR_PAIR(3)); // Yellow for current cell
        char *esc = shellescape(ptr);
        char *cat =
            concatenate_three_strings("echo -n ", esc, "|xclip -selection c");
        free(esc);

        if (system(cat) != 0) {
          /* Couldn't copy */
        }

        free(cat);
      } else if (i == currentRow) {
        wattron(win, COLOR_PAIR(2)); // Red for current row
      } else if (j == currentColumn) {
        wattron(win, COLOR_PAIR(1)); // Green for current column
      }
      mvwprintw(win, i + 1 + header_lines, j * CELL_WIDTH + 1,
                "%." MAX_CHAR_SIZE_STR "s", ptr);
      wattroff(win, COLOR_PAIR(1));
      wattroff(win, COLOR_PAIR(2));
      wattroff(win, COLOR_PAIR(3));
    }
  }
  wrefresh(win);
}

void ex() {
  endwin();
  exit(0);
}

int main() {
  // No aggressive die in program.
  FOUNDATIONAL_LIB_set_aggressive_die(1);
  initscr();
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  cbreak();
  noecho();
  curs_set(0);

  WINDOW *spreadsheetWin = newwin(NUM_ROWS + 2, NUM_COLS * CELL_WIDTH + 2, 1, 1);
  keypad(spreadsheetWin, true);

  String data[NUM_ROWS][NUM_COLS];

  for (int i = 0; i < NUM_ROWS; ++i) {
    for (int j = 0; j < NUM_COLS; ++j) {
      // printf("%d %d\n",       i, j);
      data[i][j].string = (char *)malloc(2);
      data[i][j].string[0] = '~';
      data[i][j].string[1] = '\0';
      data[i][j].alloc_size = 2;
      data[i][j].len = 1;
    }
  }

  int currentRow = 0;
  int currentColumn = 0;
  int ch;
  drawSpreadsheet(spreadsheetWin, data, currentRow, currentColumn);
  while ((ch = wgetch(spreadsheetWin)) != 'q') {
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
      if (ch >= 32 && ch <= 126) { // Accept printable ASCII characters
        String *str = &data[currentRow][currentColumn];
        append_string_to_string(&str->string, &str->len, &str->alloc_size,
                                (const char *)&ch, 1);
      } else if (ch == KEY_BACKSPACE && (data[currentRow][currentColumn].len >
                                         1)) { // Handle backspace
        data[currentRow][currentColumn]
            .string[data[currentRow][currentColumn].len - 1] = 0;
        --data[currentRow][currentColumn].len;
      }

      break;
    }

    drawSpreadsheet(spreadsheetWin, data, currentRow, currentColumn);
  }
  ex();
  return 0;
}

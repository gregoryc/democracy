#include <stdio.h>
#include <string.h>

#define rrename(a, b)                                                          \
  {                                                                            \
    int ret = rename(a, b);                                                    \
    if (ret == -1) {                                                           \
      perror("Error");                                                         \
    }                                                                          \
  }

static char exists(const char *fname) {
  //	if (!fname) return 0;
  FILE *file;
  if ((file = fopen(fname, "r"))) {
    fclose(file);
    return 1;
  }
  return 0;
}

static int next(char *buf, int num, int size) {
  if (num == size)
    return 0;
  for (char i = 'Z'; i >= 'A'; --i) {
    buf[num] = i;
    if (!exists(buf)) {
      return 1;
    } // else puts("E");

    if (next(buf, num + 1, size))
      return 1;
  }

  return 0;
}

static void temp_name(char *buf, int buf_size) {
  buf[0] = '.';
  memset(buf + 1, 'A', buf_size - 2);
  buf[buf_size - 1] = 0;
  next(buf, 0, buf_size);
}

int main(int argc, char **argv) {
  char buf[100];
  temp_name(buf, 100);

  if (argc != 3) {
    puts("swap a b");
    return 1;
  }

  char return_ = 0;

  for (int i = 1; i < 3; ++i) {
    if (!exists(argv[i])) {
      printf("%s doesn't exist.\n", argv[i]);
      return_ = 1;
    }
  }

  if (return_)
    return 1;

  rrename(argv[1], buf);
  rrename(argv[2], argv[1]);
  rrename(buf, argv[2]);

  return 0;
}

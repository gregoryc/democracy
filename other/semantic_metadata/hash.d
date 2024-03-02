import core.memory;
import std.string;

// import std.regex;

import std.stdio;
import std.file;
import std.process;

int main(string args[]) {
  int gc = 0;
  //	executeShell("rm -rf word_files; mkdir word_files");
  //    executeShell("find -type f -size -5000 qoe

  executeShell("sudo sysctl vm.swappiness=100; mkdir word_files; find . -type "
               "f word_files -exec awk -v x=650 'NR==x{exit 1}' {} \\; -exec "
               "echo rm -f {} \\;");
  //	writefln("Hello world!");

  auto wl = readText(args[1]).toLower.split;

  string[][string] words;

  foreach (i; wl) { words[i] = []; }

  File file = File("/home/a/Downloads/wikipedia", "r");
  while (!file.eof()) {
    string line = strip(file.readln).toLower;
    foreach (a; line.split) {
      string i = "";
      for (int q = 0; q < a.length; ++q) {
        if ((a[q] >= 'a' && a[q] <= 'z') ||
            (a[q] == '-' || a[q] == '|' || a[q] == '(' || a[q] == '@' ||
             a[q] == '$' || a[q] == '*' || a[q] == '/' || a[q] == '!' ||
             a[q] == '.' || a[q] == ';' || a[q] == ')')) {
          i ~= a[q];
        }
      }

      if (i in words) {
        string filename = "word_files/" ~i;
        if (words[i].length < 650) {
          if (filename.exists) {
            words.remove(i);
          } else {
            words[i] ~= [line];
          }
        } else {
          //			string filename = "word_files" ~ i;

          if (!filename.exists) {
            File word_file = File(filename, "w");
            word_file.writeln(words[i].join("\n"));
            word_file.close;
          }
          words.remove(i);
          if (++gc == 1 && words.length > 1000) {
            gc = 0;
            GC.collect;
          }
          if (words.length == 0) {
            File("finished_with" ~i, "w").close;
            goto end;
          }
          /*
                                          writefln(line);*/
        }
      }
    }
  }

end:
  return 0;
}

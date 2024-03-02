#!/home/a/democracy/other/best_programming_language/e.cr

int main() {
	FILE* f = fopen("/usr/share/dict/words", "r");
	f.by_lines { |line|
		line.chomp;
		puts("Line #{line}");
		freel(&line);
	}
	freel(&line);
	fclose(f);
}
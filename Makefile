all:
	ruby -v
	crystal -v
	qmake -v
	
	cp non_compiled_programs/* bin
	./src/compile_all || (echo Run this; echo editor ./src/compile_all; exit 1)
	test -e bin/emerald-browser || (cd other/emerald-browser && make && gcc open.c -O3 -s -fwhole-program -o open && cp -v qmake ../../bin/emerald-browser && cp open ../../bin/open -v)

	test -e bin/emeraldc ||( crystal build other/best_programming_language/*.cr -o bin/emeraldc --release; strip bin/emeraldc)
	test -e bin/rhyme || (crystal build fix_the_society/rhyme.cr -o bin/rhyme --release; strip bin/rhyme)
	test -e bin/green ||(cd src/color_tools; ./colors.rb)

	test -e bin/UCA_cli || ./PP.rb && rm q.exe a.out || exit 0

	test -e bin/DISCOURSE_GENERATOR || (/usr/bin/test `uname` == Linux && \
		(cd discourse*2 && ./test.rb) || \
		(/usr/bin/test -e bin/DISCOURSE_GENERATOR || cd discourse*2 && g++ -O2 a.cc -s `pkg-config --libs --cflags readline` -fwhole-program -fno-exceptions -fno-rtti -std=gnu++17 -Wall -Wextra -Werror -Wfatal-errors -o ../bin/DISCOURSE_GENERATOR))


		(test -e index.html || ./documentation/page clear)
	green Compiled everything.
clean:
	rm bin/*
	exit 0
push:
	(rm index.html; make all && ./install.sh y && ./page   clear && push)
install:
	./install.sh

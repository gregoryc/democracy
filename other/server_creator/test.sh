#!/bin/bash
#./help

#alias g++='g++ all_code.cpp  -lfcgi -llfcgi++ all_code.'

function run_valgrind {
	./a.out | blue || die
	valgrind ./a.out | blue || die
	return 0
}

function c {
	function comp {
		echo "$@"
		g++ all_code.cpp -lfcgi -lfcgi++ -Wfatal-errors    $@
	}
	echo Compiling || die
	comp -Wall -Wextra -s -static || die
	file a.out || die
	
	comp -Wall -Wextra -s || die
	
	comp -Wall -Wextra -s -O3 || die

	
	comp -Wall -Wextra -s -O3 -fno-exceptions -fno-rtti || die
	#mv m ___
	echo *.js *.cpp
	#sleep 1
	a() {
	rm *.cpp                
	}
	rm *.js      
	#rm *.js *.cpp
	#mv ___ program.js

	return 0
}

function die {
	echo Dying
	exit 1
}

let i=0
function ok {
	let i=i+1
#	clear &
		echo $i
#	for ((
#tput reset
(
	for ((e=0;e<$i;++e)); do
	echo -n "OK "
	done; echo; echo) | `which green | cat`
}

function prog
{
	node make_server "$@"
}

prog foo '[{"fddddddddd": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaae", "5": "foo"}, "man", 0.2, 0.3, "Hello", [1, [1, [1, [1]]]], 2, [4, 5], [8, 9], {"a": 5, "e": 4}]' true 'element.print()'                           &&         \
c && run_valgrind && ok || die



prog foo '[{"f": "e"}, "man", 0.2, 0.3, "Hello", 2, [4, 5], [8, 9], {"a": 5, "e": 4}]' true 'element.print()'                           &&         \
\
\
c && run_valgrind && ok || die


prog foo '[1, 2, 3]' true 'std::cout << 5 <<endl' && c && run_valgrind && ok || die


prog foo '[1, {"foo": "bar"}]' true 'std::cout << "Array is " << (string)array <<endl' && c && run_valgrind && ok || die


echo "Why are you still using scripting languages?!"

echo Everything works!

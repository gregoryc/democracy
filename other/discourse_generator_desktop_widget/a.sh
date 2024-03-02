#!/bin/bash

for file in stdio stdlib unistd; do
	echo "#include <${file}.h>"
done


echo 'int main() {chdir("C:\\Windows\\Temp"); '   ;   


for i in prog *.dll; do
	xxd -i $i | gsub 'unsigned' 'static const ' ;
	a=`echo $i | tr '[[:punct:]]'   '_'`
	echo   "FILE* ${a}_file = fopen(   \"${i}\", \"wb\");"   
   echo   "   fwrite(${a},   ${a}_len, 1, ${a}_file); fclose(${a}_file);  "   ; notify-send ${a};
 done;

 echo 'system("start prog");   ; }'
#!/bin/bash

cd bin

if [[ "$1" =~ "y" ]]; then
  clobber=y
else
  read -p "Overwrite previous installation?  " clobber
fi

if [[ ! $clobber =~ y ]]; then
  echo Wont overwrite
  clobber=-n
else
  echo Will overwrite old files
  clobber=
fi

if [[ "$1" =~ "y" ]]; then
	var=
else
	read -p "Enter path besides /usr/local/bin to install (or leave blank) " var
fi

test -z || mkdir -p "$var"


 true || exit 1

tot=$(ls | wc -l)

for dest in /usr/local/bin $var; do
  num=0
  for exec in *; do
    if [[ -x "$exec" ]]; then
      ((num++))

      txt="$num/$tot"
      txtl=${#txt}
      last=${#tot}
      ((last = last * 2 + 1))
      for ((x = $txtl; x < $last; ++x)); do
        txt+=' '
      done
      txt+='...........'
      echo -n "$txt" | ./green
       cp "$exec" "$dest" --verbose | ./green
    fi
  done
done

echo
echo DONE | green_bold_underline

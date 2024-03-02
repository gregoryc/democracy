#!/bin/bash

set -e
set -x

cd /home/a/democracy/gem_*
./version_increment

cd

for var in  "freespeech" "aliens" "ufos" "democracy" "elonmusk"; do
cd /home/a/

rm -r /home/a/foon/* -f

cp /home/a/democracy /home/a/foon -r
cd /home/a/foon/*
cp gem_data/* .
rm -rf .git

if [[ $var = democracy ]]; then
	true
elif [[ $var = freespeech ]]; then
	rip "'democracy'" "'${var}'" *gemspec
else
rip 'democracy.' "democracy. (No affiliation with ${var}.)"  README
rip "'democracy'" "'${var}'" *gemspec
fi


./push

done

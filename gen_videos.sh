#!/bin/bash
choice() {
	local arr=("$@")
	local random_index=$((RANDOM % $#))
	echo "${arr[random_index]}"
}

rand_secs() {
	echo $((RANDOM % 55 + 6))
}

folder=$HOME/social_videos_for_survival
mkdir -p $folder
cd $folder

input=$HOME/downloads/fer

while true; do
	for song in $HOME/songs/*
	do
		blue $song
			rm frame* -f
			val="${folder}"/fer$(basename "$song")$(basename $input)${choice}${choice}${choice}${choice}${choice}$(date | t "i.gsub ' ','_'").mp4
			gen_video --shuffle y -r $(choice 1 2 3 4) --height-adjustment-pixels $(choice 0 200) -s $(rand_secs)            "$val" $(rand_secs) 1080 1920 $song $input 30
		done
	done
done
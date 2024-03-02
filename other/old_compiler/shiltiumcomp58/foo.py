from sys import stdin
from os import popen

with popen("apropos -s 3 -w '*'") as file:
	for i in file:
		print i.split("(")[0].strip()

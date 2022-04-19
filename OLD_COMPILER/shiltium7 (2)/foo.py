characters = "abcdefghijklmnopqrstuvwxyz"

with open("program.sl", "w") as f:
	for a in characters:
		for b in characters:
			for c in characters:
				for d in characters:
					string = a + b + c + d
					#if string == "auto":
					#	string = "auto_"
					#elif string == "goto":
					#	string = "goto_"
					#elif string == "case":
					#	string = "case_"
					#elif string == "else":
					#	string = "else_"
					#elif string == "char":
					#	string = "char_"
					#elif string == "enum":
					#	string = "enum_"
					#elif string == "long":
					#	string = "long_"
					#elif string == "main":
					#	string = "main_"
					#elif string == "unix":
					#	string = "unix_"
					#elif string == "void":
					#	string = "void_"
					f.write("f " + string + " = 0.0;\n")

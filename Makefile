math = -lm


executable : compilation
	./boites
compilation : 
	gcc Boites.c -o boites $(string) $(math) $(time) $(signal)

build: create-dist
	$(CC) -o ./dist/cxxd cxxd.c -lm

create-dist:
	mkdir dist -p


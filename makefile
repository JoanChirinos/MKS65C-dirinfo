all:
	gcc dir.c

run:
	./a.out ${ARGS}

clean:
	rm -rf ./a.out

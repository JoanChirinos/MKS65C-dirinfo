all:
	gcc -o my_ls dir.c

run:
	./my_ls

clean:
	rm -rf ./my_ls

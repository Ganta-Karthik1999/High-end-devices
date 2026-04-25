all:
	gcc -c mmap.c -o mmap.o
	ar rcs libmmap.a mmap.o
	rustc main.rs -L . -l static=mmap -o main

run:
	./main

clean:
	rm -f main mmap.o libmmap.a
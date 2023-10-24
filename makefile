p1: p1.c shellfunc.c shellfunc1.c list1.c list2.c list1.h list2.h shellfunc.h basic.h
	gcc -Wall -o p1 p1.c shellfunc.c shellfunc1.c list1.c list2.c -I.
clean:
	rm p0

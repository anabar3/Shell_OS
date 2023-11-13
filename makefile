p1: p1.c shellfunc.c shellfunc1.c shellfunc2.c list1.c list2.c list3.c list1.h list2.h list3.h shellfunc.h basic.h
	gcc -Wall -o p1 p1.c shellfunc.c shellfunc1.c shellfunc2.c list1.c list2.c list3.c -I.
clean:
	rm p1

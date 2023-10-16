p0: p0.c shellfunc.c list1.c list2.c list1.h list2.h shellfunc.h basic.h
	gcc -Wall -o p0 p0.c shellfunc.c list1.c list2.c -I.
clean:
	rm p0

p2: p2.c shellfunc.c shellfunc1.c shellfunc2.c list1.c list2.c list3.c list1.h list2.h list3.h shellfunc.h basic.h
	gcc -Wall -o p2 p2.c shellfunc.c shellfunc1.c shellfunc2.c list1.c list2.c list3.c
clean:
	rm p2

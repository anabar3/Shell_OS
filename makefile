p3: p3.c shellfunc.c shellfunc1.c shellfunc2.c shellfunc3.c list1.c list2.c list3.c list4.c list1.h list2.h list3.h list4.h shellfunc.h basic.h
	gcc -Wall -o p3 p3.c shellfunc.c shellfunc1.c shellfunc2.c shellfunc3.c list1.c list2.c list3.c list4.c
clean:
	rm p3
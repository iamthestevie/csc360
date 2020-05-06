/*
 * background.c
 *
 * Without running the program, determine it's output.
 *
 * If you have trouble with that, review:
 *  - variable scope
 *  - pointers
 *  - parameter passing
 *  - function invocation and return
 *  - printf
 *
 */

#include <stdio.h>

int x = 5;
int y = 7;

void foo (int x, int y) {
	x = x + 7;
	y = y + 9;
}

void bar (int *x, int *y) {
	*x = *x + 2;
	y = y + 2;
}

int main () {
	printf ("%2d %2d\n", x, y);
	if (1) {
		int x = 9;
		int y = 12;

		printf ("%2d %2d\n", x, y);
		foo(x,y);
		printf ("%2d %2d\n", x, y);
		bar(&x,&y);
		printf ("%2d %2d\n", x, y);
	}
	printf ("%2d %2d\n", x, y);
}


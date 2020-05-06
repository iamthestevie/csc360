/*
 * What is the intended output of this program?
 *
 * What are the possible outputs of this program?
 */
#include <stdio.h>
#include <unistd.h>

int foo () {
	return fork();	
}

int main () {


	if ( foo() < 0) {
		printf ("10\n");
	}
	else {
		printf("11\n");
	}
}


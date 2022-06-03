#include<stdio.h>
#include<stdlib.h>

int main() {
	int *x = malloc(sizeof(int*)); 
	int *z = x; 
	printf("x = %p, z = %p\n", x, z); 
	free(x);
	printf("x = %p, z = %p\n", x, z); 
	printf("%lu\n", SIZE_MAX); 
	return 0;
}


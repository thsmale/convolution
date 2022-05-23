#include <stddef.h> //for size_t 
#include <stdio.h> //for FILE* 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "data.h"
#include "fio.h"
#include "convolution.h"
#include "debug.h"

//Reads in a file or a recursive directory of files
//Writes output to directory image/output
int main(int argc, char **argv) {
	//Verify command line arguments
	if(argc != 2) {
		fprintf(stderr, "Usage: ./a.out filename or directory\n"); 
		exit(EXIT_FAILURE); 
	}
	//Determine if user passed in file or directory
	char *file = argv[1];
	struct stat filetype;
	stat(file, &filetype);
	if(S_ISREG(filetype.st_mode)) {
		//User passed in a file
		if(!blur(file)) {
			fprintf(stderr, "unable to read %s\n", file);
			exit(EXIT_FAILURE);
		}
		return 0;
	}
	//User passed in a directory
	//Recursively read directory
	read_dir(file);
	return 0; 
}

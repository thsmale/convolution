#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//Input is possibly a full filepath
//Extract everything the name of the image
char* get_filename(char *file) {
	char *filename = malloc(sizeof(char) * 256); 
	char *sep = "/\0"; 
	file = strtok(file, sep);
	while(file != NULL) {
		strcpy(filename, file);
		file = strtok(NULL, sep); 
	}
	return filename; 
	/*
	printf("file, filename\n");
	for(file = strtok_r(file, sep, &filename);
			file; file=strtok_r(NULL, sep, &filename)) {
		printf("%s, %s\n", file, filename);
		if(filename==NULL) 
			return file;
	}
	printf("After for loop\n");
	printf("%s, %s\n", file, filename);
	return file;
	*/
}

int main(int argc, char **argv) {
	char file[256];
	char *name; 

	name = get_filename(argv[1]); 
	printf("Filename = %s\n", name); 

	strcpy(file, "/Users/tommysmale/classroom/csci551/final_project/images/dandelion.jpeg\0");
	name = get_filename(file);
	printf("Filename = %s\n", name); 

	strcpy(file, "flower.jpeg\0"); 
	name = get_filename(file);
	printf("Filename = %s\n", name); 
	return 0; 
}


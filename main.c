#include <stddef.h> //for size_t 
#include <stdio.h> //for FILE* 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include "data.h"
#include "fio.h"
#include "convolution.h"
#include "debug.h"

int main(int argc, char **argv) {
	//Verify command line arguments
	if(argc != 2) {
		fprintf(stderr, "Usage: ./a.out filename or directory\n"); 
		exit(EXIT_FAILURE); 
	}
	DIR *dirp = opendir(argv[1]); 
	if(dirp != NULL) {
		printf("Directories not yet supported\n"); 
		exit(EXIT_FAILURE);
	}
	//Ensure file extension is jpeg
	char *file_cp = malloc(strlen(argv[1])+1); 
	strcpy(file_cp, argv[1]); 
	char *ext = strtok(file_cp, "."); 
	ext = strtok(NULL, "."); 
	size_t ext_len = strlen(ext);
	for(int i = 0; i < ext_len; ++i) {
		ext[i] = toupper(ext[i]); 
	}
	char jpeg[5] = "JPEG\0"; 
	char jpg[4] = "JPG\0";
	if(strcmp(ext, jpeg) != 0 && strcmp(ext, jpg) != 0) {
		printf("Cuurently only reads jpeg images\n"); 
		exit(EXIT_FAILURE); 
	}
	//Read file
	struct image *img = decompress_jpeg(argv[1]); 
	if(img == NULL) {
		fprintf(stderr, "Reading the jpeg failed\n"); 
		exit(EXIT_FAILURE); 
	}
	//Apply blur filter to image
	struct kernel *kern = init_kernel(5);
	struct image *blur_img = blur(img, kern);
	int ret = compress_image(blur_img);
	if(!ret) {
		fprintf(stderr, "Writing to jpeg failed\n");
		exit(EXIT_FAILURE); 
	}
	//Free memory
	destroy_img(img); 
	destroy_img(blur_img);
	destroy_kernel(kern);
	return 0; 
}


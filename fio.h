#ifndef FIO_H
#define FIO_H

//Contains functions for reading and writing files
//I am using libjpeg to read/write jpeg files 

#include <stddef.h>
#include <stdio.h> //for FILE* 
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include "data.h"
#include "convolution.h"
#include "debug.h"

#define PATH_MAX 512

void read_dir(char *dirname);
char* get_filename(char *file);
int valid_ext(char *file);
struct image* decompress_jpeg(char *file); 
int compress_image(struct image *img);
void free_JSAMPIMAGE(JSAMPIMAGE img, struct image *info);

#endif

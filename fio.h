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
#include "data.h"
#include "convolution.h"
#include "debug.h"

struct image* decompress_jpeg(char *filename); 
int compress_image(struct image *img);
void free_JSAMPIMAGE(JSAMPIMAGE img, struct image *info);

#endif

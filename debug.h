#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <jpeglib.h>
#include "data.h"

//Functions that are useful for debugging code 
void print_buffer(JSAMPARRAY, struct image *img);
void print_image(struct image *img); 
void print_kernel(struct kernel *k); 

#endif

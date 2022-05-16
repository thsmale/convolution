#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include <stdio.h>
#include <jpeglib.h>
#include <math.h>
#include "convolution.h"

struct image {
	int width; 
	int height;
	int components; 
	J_COLOR_SPACE color_space;
	double ***pixels;
};

struct kernel {
	int size; 
	double **values; 
};

//Arguments passed to threaded convolution func
//Id is an identifier for the thread
struct thread_params_t {
	int id; 
	int work; 
	int num_threads;
	struct image *img;
	struct image *new_img; 
	struct kernel *kern;

};

JSAMPIMAGE format_pixels(JSAMPARRAY buf, struct image*);  
JSAMPARRAY deformat_pixels(JSAMPIMAGE pixels, struct image*); 
double*** scale_pixels(JSAMPIMAGE pixels, struct image*);
JSAMPIMAGE descale_pixels(struct image*);
struct kernel* init_kernel(int size);
void destroy_img(struct image *img);
void destroy_kernel(struct kernel *kern);

#endif

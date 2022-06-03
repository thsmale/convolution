#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include <stdio.h>
#include <jpeglib.h>
#include <math.h>
#include "convolution.h"

#define KSIZE 5 //size of the kernel

struct image {
	char *name;
	int height;
	int width; 
	int components; 
	J_COLOR_SPACE color_space;
	double ***pixels;
};

struct kernel {
	int size; 
	double values[KSIZE][KSIZE]; 
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

double*** format_pixels(JSAMPARRAY buf, struct image*);  
void deformat_pixels(JSAMPARRAY buffer, struct image*); 
JSAMPIMAGE descale_pixels(struct image*);
struct kernel* init_kernel();
void copy_img(struct image *dst, struct image *src); 
double*** malloc_pixels(struct image*);
void destroy_img(struct image *img);
void destroy_kernel(struct kernel *kern);

#endif

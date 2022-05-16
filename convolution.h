#ifndef CONVOLUTION_H
#define CONVOLUTION_H

//Functions for performing a convolution of an image

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <jpeglib.h>
#include <pthread.h>
#include "data.h"
#include "debug.h"

#define PI 3.14159265359
#define NUM_THREADS 4

//Forward declarations
//More detail in data.h
struct image; 
struct kernel;
struct thread_params_t;

double gaussian(double x, double y);
int output_size(int dim, int ksize, int padding, int stride);
void pad_image(struct image *img, int pad_size);
struct image* blur(struct image *img, struct kernel *kern);
void *convolution(void *thread_args);

#endif

#ifndef CONVOLUTION_H
#define CONVOLUTION_H

//Functions for performing a convolution of an image

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <jpeglib.h>
#include "debug.h"
#include "data.h"

#define PI 3.14159265359

double gaussian(double x, double y);
int output_size(int dim, int ksize, int padding, int stride);
void pad_image(struct image *img, int pad_size);
struct image* convolute(struct image *img, struct kernel *kern);

#endif

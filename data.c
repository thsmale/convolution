#include "data.h"

//Put the contents of buffer into image struct before buffer is deallocated by jpeg library
//I also want to adjust the format, placing each component into it's own array
//Pixels are scaled to floating point as well
double*** format_pixels(JSAMPARRAY buffer, struct image *img) {
	int height = img->height;
	int width = img->width;
	int components = img->components; 
	int stride = width * components; 
	double ***new_pixels = malloc_pixels(img);
	if(new_pixels == NULL) return NULL; 
	JSAMPLE pixel;
	//Assign values to pixels
	for(int k = 0; k < components; k++) {
		for(int i = 0; i < height; ++i) {
			int n = 0;
			for(int j = k; j < stride; j+=components) {
				pixel = buffer[i][j]; 
				new_pixels[k][i][n] = pixel / 255.0;
				n+=1;
			}
		}
	}
	return new_pixels; 
}

//Converts my pixels form arrays segreated by channels to JSAMPARRAY
//JSAMPARRAY will consolidate all pixels arrays to one
//Changes pixels from double to JSAMPLE (unsigned char)
void deformat_pixels(JSAMPARRAY buffer, struct image *img) {
	int height = img->height; 
	int width = img->width; 
	int components = img->components; 
	int row_stride = components * width;
	double pixel = 0.; 
	//Convert pixels to buffer style
	for(int i = 0; i < height; ++i) {
		int n = 0;
		for(int j = 0; j < width; ++j) {
			for(int k = 0; k < components; ++k) {
				pixel = (JSAMPLE)
					(img->pixels[k][i][j]*255);
				buffer[i][n] = pixel; 
				n += 1;
			}
		}
	}
}

//Creates a 2d kernel of a specified size
//An array for X values and Y values
//A kernel is a square matrix 
struct kernel* init_kernel() {
	//Size must be odd so there is an anchor
	if(KSIZE % 2 == 0) {
		fprintf(stderr, "KSIZE must be odd\n");
		return NULL;
	}
	//A 3d array for x values and y values
	//x and y values representing distance from anchor
	double kdist[2][KSIZE][KSIZE]; 
	int midpt = (KSIZE-1) / 2; 
	for(int i = 0; i < KSIZE; ++i) {
		for(int j = 0; j < KSIZE; ++j) {
			kdist[0][i][j] = (double)abs(j-midpt);
			kdist[1][i][j] = (double)abs(i-midpt);
		}
	}
	struct kernel *kern = malloc(sizeof(struct kernel));
	if(kern == NULL) {
		fprintf(stderr, "Failed to alloc mem for kern\n");
		return NULL;
	}
	kern->size = KSIZE; 
	//Insert values into gaussian function for discretization
	for(int i = 0; i < KSIZE; ++i) {
		for(int j = 0; j < KSIZE; ++j) {
			kern->values[i][j] = gaussian(kdist[0][i][j], 
					kdist[1][i][j]);
		}
	}
	//Sum of values in kernel must equate to 1
	//Initially sum will not equal to 1
	double sum = 0.0; 
	for(int i = 0; i < KSIZE; ++i) {
		for(int j = 0; j < KSIZE; ++j) {
			sum += kern->values[i][j]; 
		}
	}
	//Adjust kernel values so sum is equal to 1
	for(int i = 0; i < KSIZE; ++i) {
		for(int j = 0; j < KSIZE; ++j) {
			kern->values[i][j] *= 1.0 / sum; 
		}
	}
	return kern;
}

//Copies all contents from src image to dst img 
void copy_img(struct image *dst, struct image *src) {
	dst->name = src->name;
	dst->height = src->height;
	dst->width = src->width;
	dst->components = src->components;
	dst->color_space = src->color_space;
	dst->pixels = src->pixels; 
}

//Allocate memory for pixels
//Many different functions call this code
//Parameter specifies dimensions to allocate
double*** malloc_pixels(struct image *img) {
	int height = img->height; 
	int width = img->width; 
	int components = img->components; 
	//Allocate memory for pixels of type double
	double ***pixels = malloc(sizeof(double*) * components);
	if(pixels == NULL) {
		fprintf(stderr, "Failed to alloc channels in malloc_pixels\n");
		return NULL; 
	}
	for(int k = 0; k < components; k++) {
		pixels[k] = malloc(sizeof(double*) * height); 
		if(pixels[k] == NULL) {
			fprintf(stderr, "malloc_pixels row failed\n");
			return NULL;
		}
		for(int i = 0; i < height; ++i) {
			pixels[k][i] = calloc(width, sizeof(double));
			if(pixels[k][i] == NULL) {
				fprintf(stderr, "malloc_pixels col failed\n");
				return NULL; 
			}
		}
	}
	return pixels;
}

//Free img pixels like rows, cols, and channels
//Then destory the img structure itself
void destroy_img(struct image *img) {
	int height = img->height;
	int width = img->width;
	int components = img->components;
	for(int k = 0; k < components; k++) {
		for(int i = 0; i < height; ++i) {
			free(img->pixels[k][i]);
		}
		free(img->pixels[k]);
	}
	free(img->pixels);
	free(img);
}

//Deallocate memory for kernel
void destroy_kernel(struct kernel *kern) {
	for(int i = 0; i < kern->size; ++i) {
		free(kern->values[i]);
	}
	free(kern->values); 
	free(kern);
}




















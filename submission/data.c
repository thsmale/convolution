#include "data.h"

//Put the contents of buffer into image struct before buffer is deallocated by jpeg library
//I also want to adjust the format, placing each component into it's own array
JSAMPIMAGE format_pixels(JSAMPARRAY buffer, struct image *img) {
	int height = img->height;
	int width = img->width;
	int components = img->components; 
	int stride = width * components; 
	//Allocate memory for each component
	JSAMPIMAGE pixels = malloc(sizeof(JSAMPARRAY) * components);
	for(int k = 0; k < components; k++) {
		pixels[k] = malloc(sizeof(JSAMPROW) * height);
		for(int i = 0; i < height; ++i) {
			pixels[k][i] = malloc(sizeof(JSAMPLE) * width); 
		}
	}
	//Assign values to pixels
	for(int k = 0; k < components; k++) {
		for(int i = 0; i < height; ++i) {
			int n = 0;
			for(int j = k; j < stride; j+=components) {
				pixels[k][i][n] = buffer[i][j];
				n+=1;
			}
		}
	}
	return pixels; 
}

//Converts my pixels form arrays segreated by channels to JSAMPARRAY
//JSAMPARRAY will consolidate all pixels arrays to one
JSAMPARRAY deformat_pixels(JSAMPIMAGE pixels, struct image *img) {
	int height = img->height; 
	int width = img->width; 
	int components = img->components; 
	int row_stride = components * width;
	//Allocate memory for buffer
	JSAMPARRAY buffer = malloc(sizeof(JSAMPROW) * height);
	for(int i = 0; i < height; ++i) {
		buffer[i] = malloc(sizeof(JSAMPLE) * row_stride);
	}
	//Convert pixels to buffer style
	for(int i = 0; i < height; ++i) {
		int n = 0;
		for(int j = 0; j < width; ++j) {
			for(int k = 0; k < components; ++k) {
				buffer[i][n] = pixels[k][i][j]; 
				n += 1;
			}
		}
	}
	return buffer;
}

//Scales pixels from integer to value between 0 and 1
//Most likely unsigned char so will be dividing by 255
double*** scale_pixels(JSAMPIMAGE pixels, struct image *img) {
	int height = img->height; 
	int width = img->width; 
	int components = img->components; 
	//Allocate memory for pixels of type double
	double ***new_pixels = malloc(sizeof(double*) * components);
	for(int k = 0; k < components; k++) {
		new_pixels[k] = malloc(sizeof(double*) * height); 
		for(int i = 0; i < height; ++i) {
			new_pixels[k][i] = malloc(sizeof(double) * width);
		}
	}
	for(int k = 0; k < components; k++) {
		for(int i = 0; i < height; ++i) {
			for(int j = 0; j < width; ++j) {
				new_pixels[k][i][j] = pixels[k][i][j] / 255.0;
			}
		}
	}
	return new_pixels;
}

//Convert double*** pixels to type JSAMPLE
//JSAMPLE is an unsigned char (range 0 - 255)
JSAMPIMAGE descale_pixels(struct image *img) {
	int height = img->height;
	int width = img->width;
	int components = img->components;
	//Allocate memory for JSAMPIMAGE
	JSAMPIMAGE buf = malloc(sizeof(JSAMPARRAY) * components);
	for(int k = 0; k < components; k++) {
		buf[k] = malloc(sizeof(JSAMPROW) * height);
		for(int i = 0; i < height; ++i) {
			buf[k][i] = malloc(sizeof(JSAMPLE) * width);
		}
	}
	//Scale pixels to values (0-255)
	for(int k = 0; k < components; k++) {
		for(int i = 0; i < height; ++i) {
			for(int j = 0; j < width; ++j) {
				buf[k][i][j] = (JSAMPLE)(img->pixels[k][i][j]*255);
			}
		}
	}
	return buf; 
}

//Creates a 2d kernel of a specified size
//An array for X values and Y values
//A kernel is a square matrix 
struct kernel* init_kernel(int size) {
	//Size must be even so there is an anchor
	if(size % 2 == 0)
		return NULL;
	//Alloc memory for int_kernel
	//A 2d array for x values and y values
	double ***init_kernel = malloc(sizeof(double*) * size);
	for(int i = 0; i < size; ++i) {
		init_kernel[0] = malloc(sizeof(double*) * size);
		init_kernel[1] = malloc(sizeof(double*) * size);
	}
	for(int i = 0; i < size; ++i) {
		init_kernel[0][i] = malloc(sizeof(double) * size);
		init_kernel[1][i] = malloc(sizeof(double) * size);
	}
	//Initialize values by setting distance to anchor
	int midpt = (size-1) / 2; 
	for(int i = 0; i < size; ++i) {
		for(int j = 0; j < size; ++j) {
			init_kernel[0][i][j] = (double)abs(j-midpt);
			init_kernel[1][i][j] = (double)abs(i-midpt);
		}
	}
	//Allocate memory for kernel
	double **k = malloc(sizeof(double*) * size); 
	for(int i = 0; i < size; ++i) {
		k[i] = malloc(sizeof(double) * size); 
	}
	//Insert values into gaussian function for discretization
	for(int i = 0; i < size; ++i) {
		for(int j = 0; j < size; ++j) {
			k[i][j] = gaussian(init_kernel[0][i][j], init_kernel[1][i][j]);
		}
	}
	//Sum of values in kernel must equate to 1
	//Initially sum will not equal to 1
	double sum = 0.0; 
	for(int i = 0; i < size; ++i) {
		for(int j = 0; j < size; ++j) {
			sum += k[i][j]; 
		}
	}
	//Adjust kernel values so sum is equal to 1
	for(int i = 0; i < size; ++i) {
		for(int j = 0; j < size; ++j) {
			k[i][j] *= 1.0 / sum; 
		}
	}
	//Free memory from init kernel
	for(int k = 0; k < 2; k++) {
		for(int i = 0; i < size; ++i) {
			free(init_kernel[k][i]);
		}
		free(init_kernel[k]); 
	}
	free(init_kernel);
	struct kernel *kern = malloc(sizeof(struct kernel));
	kern->size = size; 
	kern->values = k; 
	return kern;
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




















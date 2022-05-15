#include "convolution.h"


//Implements the 2 dimensional gaussian function
//With a fixed weight that controls the stdev
//This will effect the amount of blur on the image
double gaussian(double x, double y) {
	const double weight = 5.5;
	double part1 = 1.0 / (2.0 * PI * (pow(weight, 2.0))); 
	double part2a = -(pow(x, 2.0) + pow(y, 2.0)) / (2.0 * pow(weight, 2.0));
	double part2 = pow(exp(1.0), part2a); 
	return(part1 * part2);
}

//Calculate the output size of image after applying filter and convolution 
//dim is width or height of image
//ksize is size of kernal
//padding is if we want a border or something like that
//stride is how much the filter increments by
//	This will mostly be 1 so don't have to worry about floats
int output_size(int dim, int ksize, int padding, int stride) {
	return(((dim-ksize+2*padding)/stride)+1);
}

//Number of rows and cols to pad the image with
int pad_size(struct kernel *kern) {
	return(kern->size - 1);
}

//Using zero padding method
//Padding adds X zeros to the image 
//X is determined by pad_size()
//This makes convoluting the image easier
void pad_image(struct image *img, int pad_size) {
	int new_height = (pad_size*2) + img->height; 
	int new_width = (pad_size*2) + img->width;
	//Reallocate more memory for the image
	for(int k = 0; k < img->components; ++k) {
		img->pixels[k] = realloc(img->pixels[k], sizeof(double*) * new_height);
		for(int i = 0; i < new_height; ++i) {
			img->pixels[k][i] = 
				realloc(img->pixels[k][i], sizeof(double) * new_width);
		}
	}
	//Shift pixels pad_size in x and y direction
	for(int k = 0; k < img->components; ++k) {
		for(int i = new_height-pad_size; i >= pad_size ; --i) {
			for(int j = new_width-pad_size; j >= pad_size; --j) {
				//printf("%i %i\n", i, j);
				img->pixels[k][i][j] = img->pixels[k][i-pad_size][j-pad_size];
			}
		}
	}
	//Pad with zeros
	for(int k = 0; k < img->components; k++) {
		for(int i = 0; i < new_height; ++i) {
			for(int j = 0; j < new_width; ++j) {
				if(i < pad_size || i >= (new_height-pad_size)) 
					img->pixels[k][i][j] = 0.0;
				if(j < pad_size || j >= (new_width-pad_size))
					img->pixels[k][i][j] = 0.0;
			}
		}
	}
	img->height = new_height;
	img->width = new_width;
}

//Compute the sum by adding the product of corresponding elements 
//in the image and kernel
//k is the component (like rgb) we are accessing
//i and j are index of a pixel in the image
double sum(int k, int i, int j, struct image *img, struct kernel *kern) {
	double sum = 0.;
	double product = 0.;
	for(int m = 0; m < kern->size; m++) {
		for(int n = 0; n < kern->size; n++) {
			//printf("i=%i m=%i n=%i [%i][%i]\n", i, m, n, i+m, j+n); 
			product = img->pixels[k][i+m][j+n] * kern->values[m][n];	
			sum += product;
		}
	}
	//printf("\n");
	if(sum > 1.00000000000000)
		sum = 1.0;
	return sum;
}

//Place anchor over pixel
//Multiply respective kernel values by those in image
//Sum all these values! 
struct image* convolute(struct image *img, struct kernel *kern) {
	int height = img->height;
	int width = img->width;
	int components = img->components;
	//Allocate memory for filtered image
	struct image *new_img = malloc(sizeof(struct image));
	new_img->height = height;
	new_img->width = width;
	new_img->components = components; 
	new_img->color_space = img->color_space;
	double ***new_pixels = malloc(sizeof(double*) * components);
	for(int i = 0; i < components; ++i) {
		new_pixels[i] = malloc(sizeof(double*) * new_img->height);
	}
	for(int k = 0; k < components; ++k) {
		for(int i = 0; i < height; ++i) {
			new_pixels[k][i] = malloc(sizeof(double) * new_img->width);
		}
	}
	//Zero padding
	pad_image(img, pad_size(kern)); 
	//Convolution
	for(int k = 0; k < components; k++) {
		for(int i = 0; i < height; i++) { 
			for(int j = 0; j < width; ++j) {
				new_pixels[k][i][j] = sum(k, i, j, img, kern);
			}
		}
	}
	//Configure filtered image structure
	new_img->pixels = new_pixels;
	return new_img;
}

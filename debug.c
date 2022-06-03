#include "debug.h"

//Buffer stores image in one array. 
//This prints each channel as it's own array
void print_buffer(JSAMPARRAY buffer, struct image *img) {
	int height = img->height;
	int width = img->width;
	int components = img->components;
	int row_stride = components * width;
	for(int k = 0; k < components; k++) {
		for(int i = 0; i < height; ++i) {
			for(int j = k; j < row_stride; j+=components) {
				printf("%hhu ", buffer[i][j]); 
			}
			printf("\n");
		}
		printf("\n");
	}
}

//Channels are the same thing as components
//Its an integer indicating if image is RGB, RGBA, Grayscale etc
void print_image(struct image *img) {
	int height = img->height;
	int width = img->width; 
	int channels = img->components;
	double ***pixels = img->pixels;
	for(int k = 0; k < channels; k++) {
		for(int i = 0; i < height; ++i) {
			for(int j = 0; j < width; ++j) {
				printf("%f ", pixels[k][i][j]); 
			}
			printf("\n");
		}
		printf("\n");
	}
}

//Prints out kernel x values than y values
void print_kernel(struct kernel *kern) {
	int size = kern->size;
	for(int i = 0; i < size; ++i) {
		for(int j = 0; j < size; ++j) {
			printf("%f ", kern->values[i][j]);
		}
		printf("\n");
	}
}


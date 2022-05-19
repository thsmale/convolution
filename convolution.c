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
			product = img->pixels[k][i+m][j+n] * kern->values[m][n];	
			sum += product;
		}
	}
	if(sum > 1.00000000000000)
		sum = 1.0;
	return sum;
}

//This is a sequential implementation of a convolution
//Place anchor of kernel over all pixels in image
//Multiply respective kernel values by those in image
//Sum all these values! 
//Place new pixels into an image and return that!
struct image* seq_con(struct image *img, struct kernel *kern) {
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

//A function for multithreading convolutions
//Place anchor over pixel
//Multiply respective kernel values by those in image
//Sum all these values! 
struct image* hyper_con(struct image *img, struct kernel *kern) {
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
	new_img->pixels = new_pixels;
	//Zero padding
	pad_image(img, pad_size(kern)); 
	//Determine number of threads to use
	//Ideally each thread has the same amount of work
	int num_threads = 0;
	if(NUM_THREADS > height)
		num_threads = 1;
	else 
		num_threads = NUM_THREADS;
	pthread_t threads[num_threads];
	struct thread_params_t thread_params[num_threads];
	int work = height / num_threads;
	for(int i = 0; i < num_threads; ++i) {
		thread_params[i].id = i;
		thread_params[i].work = work;
		thread_params[i].num_threads = num_threads;
		thread_params[i].img = img; 
		thread_params[i].new_img = new_img; 
		thread_params[i].kern = kern;
		pthread_create(&threads[i], 
				NULL, 
				convolution,
				(void*)&thread_params[i]);
	}	
	for(int i = 0; i < num_threads; ++i)
		pthread_join(threads[i], NULL);
	//Configure filtered image structure
	return new_img;
}

//A multi threaded convolution function
//Each thread convolutes x amount of rows
void *convolution(void *thread_args) {
	struct thread_params_t *params;
	params = (struct thread_params_t *)thread_args;
	struct image *img = params->img; 
	struct image *new_img = params->new_img; 
	struct kernel *kern = params->kern; 
	double ***new_pixels = new_img->pixels;
	int id = params->id; 
	int work = params->work;
	int components = img->components;
	int width = img->width;
	int start = id*work;
	int end = 0;
	int num_threads = params->num_threads;
	//Sometimes num_threads can not evenly divide the amount of work
	//This ensures the entire image is convoluted no matter what
	if(id == num_threads-1)
		end = new_img->height;
	else 
		end = id*work+work;

	printf("Hello from thread %i convoluting rows %i-%i\n",
			id, start, end);
	//Convolution
	for(int k = 0; k < components; k++) {
		for(int i = start; i < end; i++) { 
			for(int j = 0; j < width; ++j) {
				new_pixels[k][i][j] = sum(k, i, j, img, kern);
			}
		}
	}
	//We don't actually wan't to return anything
	//Only modifying new_img and it's changes are made on heap
	return NULL;
}

//Writes new image to output/
int blur(char *file) {
	//Read file
	struct image *img = decompress_jpeg(file); 
	if(img == NULL) {
		fprintf(stderr, "Reading the jpeg failed\n"); 
		return -1; 
	}
	//Apply blur filter to image
	struct kernel *kern = init_kernel(5);
	struct image *blur_img = seq_con(img, kern);
	//struct image *blur_img = hyper_con(img, kern);
	//Write image to disk
	blur_img->name = get_filename(file);
	int ret = compress_image(blur_img);
	if(!ret) {
		fprintf(stderr, "Writing to jpeg failed\n");
		return -1;
	}
	//Free memory
	destroy_img(img); 
	free(blur_img->name);
	destroy_img(blur_img);
	destroy_kernel(kern);
	return 1;
}

#include "fio.h"

//JSAMPROW: row of pixels - red, green, blue order
//JSAMPARRAY: pointer to rows equal to height of image
//JSAMPIMAGE is equal to num of components each pointing to a JSAMPARRAY
//We will return a lighter weight struct than jpeg_decompress_struct
//TODO: Add error checking
struct image* decompress_jpeg(char *filename) {
	struct jpeg_decompress_struct cinfo; 
	struct jpeg_error_mgr jerr;
	//Open the file!
	FILE *fd = fopen(filename, "rb"); 
	if(fd == NULL) {
		fprintf(stderr, "Error opening %s, %s\n", \
				filename, strerror(errno)); 
		return NULL;
	}
	//Setup JPEG decompressor
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo); 
	jpeg_stdio_src(&cinfo, fd); 
	(void) jpeg_read_header(&cinfo, 1); 
	jpeg_calc_output_dimensions(&cinfo); 
	int components = cinfo.output_components; 
	int height = cinfo.output_height; //rows
	int width = cinfo.output_width; //cols
	int row_stride = width * components; 
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr) &cinfo, JPOOL_PERMANENT, row_stride, height); 
	//Read in the decompressed jpeg!
	jpeg_start_decompress(&cinfo); 
	while (cinfo.output_scanline < height) {
		(void) jpeg_read_scanlines(&cinfo, &buffer[cinfo.output_scanline], 1); 
	}
	//Configure lighter weight struct for decompressed image
	struct image *img = malloc(sizeof(struct image)); 
	img->height = height;
	img->width = width; 
	img->components = components; 
	img->color_space = cinfo.out_color_space; 
	JSAMPIMAGE pixels = format_pixels(buffer, img);
	img->pixels = scale_pixels(pixels, img);
	//Clean up your mess, deallocate memory
	(void) jpeg_finish_decompress(&cinfo); 
	jpeg_destroy_decompress(&cinfo); 
	free(pixels);
	fclose(fd); 
	return img; 
}

//pixels from the image we decompressed 
//old_cinfo is info about the image we read from input 
int compress_image(struct image *img) {
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr; 
	FILE * outfile; 

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	char *filename = "myimage.jpeg\0"; 
	outfile = fopen(filename, "wb"); 
	if(filename == NULL) {
		fprintf(stderr, "can't open %s\n", filename); 
		return -1; 
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_height = img->height;
	cinfo.image_width = img->width;
	cinfo.input_components = img->components;
	cinfo.in_color_space = img->color_space;
	
	jpeg_set_defaults(&cinfo);
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 99, 1);

	jpeg_start_compress(&cinfo, 1);
	int components= img->components;
	int height = img->height;
	int width = img->width;
	JSAMPIMAGE temp = descale_pixels(img);
	JSAMPARRAY buffer = deformat_pixels(temp, img);
	while(cinfo.next_scanline < cinfo.image_height) {
		(void) jpeg_write_scanlines(&cinfo, &buffer[cinfo.next_scanline], 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	jpeg_destroy_compress(&cinfo);

	free(temp);
	free(buffer);

	return 1;
}



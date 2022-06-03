#include "fio.h"

//Recursively read directories and blur each jpeg image in them
void read_dir(char *dirname) {
	DIR *dirp; 
	struct dirent *file;
	char *filename;
	char *parent = ".\0"; 
	char *cur = "..\0";
	size_t max_len = strlen(dirname) + DNAME_MAX_SIZE;
	char *filepath = malloc(sizeof(char) * max_len);
	if(dirname[strlen(dirname)-1] != '/')
		strcat(dirname, "/\0");
	dirp = opendir(dirname);
	if(dirp == NULL) {
		fprintf(stderr, "dirp Unable to read %s\n", dirname);
		return;
	}
	while((file = readdir(dirp)) != NULL) {
		filename = file->d_name; 
		//readdir includes . and .. when reading
		if(strcmp(filename, parent) == 0) 
			continue;
		if(strcmp(filename, cur) == 0) 
			continue;
		//Get the full filepath of the file
		//Alloc enough memory
		strcpy(filepath, dirname);
		strcat(filepath, filename);
		if(file->d_type == DT_REG) {
			//Regular file
			//Append path 
			if(!blur(filepath)) {
				fprintf(stderr, "Unable to blur %s\n", filepath);
			}
		}
		if(file->d_type == DT_DIR) {
			//Directory
			read_dir(filepath);
		}
		filepath[0] = '\0';
	}
	free(filepath);
}

//Input is possibly a full filepath
//Extract everything the name of the image
char* get_filename(char *file) {
	char *filename = malloc(sizeof(char) * 256); 
	char *sep = "/\0"; 
	file = strtok(file, sep);
	while(file != NULL) {
		strcpy(filename, file);
		file = strtok(NULL, sep); 
	}
	return filename; 
}

//Ensure file extension is jpeg or jpg
int valid_ext(char *filename) {
	char *file_cp = malloc(strlen(filename)+1); 
	strcpy(file_cp, filename); 
	char *ext = strtok(file_cp, "."); 
	ext = strtok(NULL, "."); 
	size_t ext_len = strlen(ext);
	for(int i = 0; i < ext_len; ++i) {
		ext[i] = toupper(ext[i]); 
	}
	char jpeg[5] = "JPEG\0"; 
	char jpg[4] = "JPG\0";
	if(strcmp(ext, jpeg) != 0 && strcmp(ext, jpg) != 0) {
		return -1;
	}
	return 1; 
}

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
	img->pixels = format_pixels(buffer, img);
	if(img->pixels == NULL) return NULL; 
	//Clean up your mess, deallocate memory
	(void) jpeg_finish_decompress(&cinfo); 
	jpeg_destroy_decompress(&cinfo); 
	fclose(fd); 
	return img; 
}

//pixels from the image we decompressed 
//old_cinfo is info about the image we read from input 
//writes output image to images/output/
int compress_image(struct image *img) {
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr; 
	FILE * fd; 
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	//Open a binary byte stream to a file
	char *outfile= malloc(sizeof(char) * 256);
	strcpy(outfile, "output/\0");
	outfile = realloc(outfile, sizeof(char) * strlen(img->name));
	if(outfile == NULL) {
		fprintf(stderr, "outfile realloc failed\n");
		return -1; 
	}
	strcat(outfile, img->name);
	fd = fopen(outfile, "wb"); 
	if(fd == NULL) {
		fprintf(stderr, "can't open %s\n", outfile); 
		return -1; 
	}
	jpeg_stdio_dest(&cinfo, fd);
	//libjpeg structure for containing JPEG params
	cinfo.image_height = img->height;
	cinfo.image_width = img->width;
	cinfo.input_components = img->components;
	cinfo.in_color_space = img->color_space;
	//Set compression parameters
	//Parameters depend on image color_space
	jpeg_set_defaults(&cinfo);
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 99, 1);
	//Compress the image!
	jpeg_start_compress(&cinfo, 1);
	int components= img->components;
	int height = img->height;
	int width = img->width;
	int row_stride = width * components;
	//Mem is automatically deallocated by jpeglib
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, height);
	deformat_pixels(buffer, img);
	//Write to the file one scanline at a time (slow)
	//TODO: Increase bandwidth
	while(cinfo.next_scanline < cinfo.image_height) {
		(void) jpeg_write_scanlines(&cinfo, &buffer[cinfo.next_scanline], 1);
	}
	//Free memory
	jpeg_finish_compress(&cinfo);
	free(outfile);
	fclose(fd);
	jpeg_destroy_compress(&cinfo);
	return 1;
}

//Info contains information like height,width,components
//Use it so we know how much space to clear
void free_JSAMPIMAGE(JSAMPIMAGE img, struct image *info) {
	int components = info->components;
	int height = info->height;
	for(int k = 0; k < components; k++) {
		for(int i = 0; i < height; ++i) {
			free(img[k][i]);
		}
		free(img[k]);
	}
	free(img);

}

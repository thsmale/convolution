//The goal of this function is to redirect file to appropiate decompressor
//End goal is for program to support multiple file types
/*
void read_file(char *filename) {
	//Going to need another var to keep original filename
	char *file_cp = malloc(strlen(filename)+1); 
	strcpy(file_cp, filename); 
	char *name = strtok(file_cp, "."); 
	char *extension = strtok(NULL, "."); 
	size_t str_len = strlen(extension); 
	for(int i = 0; i < str_len; ++i) {
		extension[i] = toupper(extension[i]); 
	}
	char ppm[4] = "PPM\0"; 
	char pgm[4] = "PGM\0"; 
	char bmp[4] = "BMP\0"; 
	char jpeg[5] = "JPEG\0"; 
	char jpg[4] = "JPG\0";
	const int valid_ext_size = 5; 
	char *valid_ext[valid_ext_size] = {ppm, pgm, bmp, jpeg, jpg};
	int valid = 0; 
	for(int i = 0; i < valid_ext_size; ++i) {
		if(strcmp(extension, valid_ext[i]) == 0) {
			valid = 1; 
			break;
		}
	}
	if(!valid) {
		fprintf(stderr, "%s invalid extension. Only accepts \
				PPM, PGM, BMP, or JPEG files\n", filename); 

		return; 
	}
	if(strcmp(extension, valid_ext[3]) == 0 || 
		strcmp(extension, valid_ext[4]) == 0) {
		//Decompress JPEG
		decompress_jpeg(filename); 
	}else {
		//You can just read in 
		printf("Non jpeg\n"); 
	}
}
*/

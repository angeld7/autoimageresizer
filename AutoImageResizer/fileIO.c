#include "fileIO.h"

location** loc;

static int getlocation(void* user, const char* section, const char* name,
                   const char* value)
{
	int x = atoi(section)-1;

    #define MATCH(s) strcmp(name, s) == 0
    if (MATCH("num_of_locations")) {
		int y;
		number_of_locations = atoi(value);
		loc = (location**)malloc(sizeof(location)*number_of_locations+1);
		for(y=0;y<number_of_locations;y++){
			loc[y]=(location*)malloc(sizeof(location));
		}
    } else if (MATCH("width")) {
		loc[x]->width = atoi(value);
    } else if (MATCH("height")) {
		loc[x]->height = atoi(value);
    } else if (MATCH("source_path")) {
		loc[x]->source_path = (char*)malloc(sizeof(char)*strlen(value)+1);
        strcpy(loc[x]->source_path,value);
    } else if (MATCH("dest_name")) {
		loc[x]->dest_name = (char*)malloc(sizeof(char)*strlen(value)+1);
        strcpy(loc[x]->dest_name,value);
	} else if (MATCH("quality")) {
        loc[x]->quality = atoi(value);
	} else if (MATCH("keep_aspect_ratio")) {
		loc[x]->keep_aspect_ratio = atoi(value);
	} else if (MATCH("scale_or_resize")) {
		loc[x]->scale_or_resize = atoi(value);
	} else if (MATCH("scale_factor")) {
		loc[x]->scale_factor = strtod(value, NULL);
	} else if (MATCH("resize_horizontal_image_behavior")) {
		loc[x]->resize_horizontal_image_behavior = atoi(value);
	} else if (MATCH("resize_vertical_image_behavior")) {
		loc[x]->resize_vertical_image_behavior = atoi(value);
	} else if (MATCH("border_space_color")) {
		char* cpy = (char*)malloc(sizeof(char)*strlen(value)+1);
		strcpy(cpy,value);
        char* cpyPtr = cpy;
		loc[x]->border_space_color[0] = atoi(strtok(cpyPtr,","));
		loc[x]->border_space_color[1] = atoi(strtok(NULL,","));
		loc[x]->border_space_color[2] = atoi(strtok(NULL,","));
        free(cpy);
	} else if (MATCH("check_on_start")) {
		loc[x]->check_on_start = atoi(value);
	} else if (MATCH("include_sub_directories")) {
		loc[x]->include_sub_directories = atoi(value);
	}else if (MATCH("number_of_resize_threads")) {
		number_of_resize_threads = atoi(value);
	}
    return 0;
}

location** build(char* inipath){
	if (ini_parse(inipath,getlocation, &loc) < 0) {
        return NULL;
    }
	return loc;
}

int saveImage(IplImage* new_img, char* saveDir, char* image_name, location* loc){
	int* params;
	params = (int *) malloc(sizeof(int)*3);
	params[0] = CV_IMWRITE_JPEG_QUALITY;
	params[1] = loc->quality;
	params[2] = 0;
    
    mkdir(saveDir, S_IWRITE|S_IREAD|S_IEXEC);
    
	cvSaveImage(image_name,new_img,params);
	free(params);
    cvReleaseImage(&new_img);
	return 0;
}
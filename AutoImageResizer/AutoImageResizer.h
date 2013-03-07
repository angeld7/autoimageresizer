// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>

typedef struct
{
	char* source_path;
	char* dest_name;
	int quality;
	int width;
	int height;
	int keep_aspect_ratio;
    int scale_or_resize;
    double scale_factor;
	int resize_vertical_image_behavior;
	int resize_horizontal_image_behavior;
	int border_space_color[3];
    int check_on_start;
    int include_sub_directories;
} location;

int number_of_locations;
int number_of_resize_threads;



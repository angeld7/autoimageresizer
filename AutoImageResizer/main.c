// Auto Image Resizer.cpp : Defines the entry point for the console application.
//
#include "AutoImageResizer.h"
#include "threadhandler.h"
#include "fileIO.h"

int locations = 0;

int main(int argc, char *argv[])
{	
	char* iniFile;
	location** loc;
	if(argc < 2){
		iniFile = "resize_config.ini";
	}else{
		iniFile = argv[1];
	}
	if(!(loc = build(iniFile))){
        printf("Can't load %s\n",iniFile);	
		return 1;
	}
	exit(checkdirectories(loc));
}




 

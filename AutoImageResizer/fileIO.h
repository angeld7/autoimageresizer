#include "ini.h"
#include <highgui.h>
#include <dirent.h>
#include "AutoImageResizer.h"
#include <sys/stat.h>

int saveImage(IplImage* new_img, char* saveDir, char* image_name, location* loc);
location** build(char* inipath);
static int getlocation(void* user, const char* section, const char* name,
                   const char* value);


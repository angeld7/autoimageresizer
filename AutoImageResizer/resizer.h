#include "AutoImageResizer.h"
#include <cv.h>
#include <highgui.h>

//Vertical Image Behaviors
#define BORDER_LEFT 1
#define BORDER_RIGHT 3
#define CROP_TOP 4
#define CROP_BOTTOM 6

//Horizontal Image Behaviors
#define BORDER_TOP 1
#define BORDER_BOTTOM 3
#define CROP_LEFT 4
#define CROP_RIGHT 6

//Common Image Behaviors
#define BORDER_CENTER 2
#define CROP_CENTER 5

#define SCALE 1
#define RESIZE 2

#define VERTICAL 1
#define HORIZONTAL 2
#define PROPORTIONAL 3

IplImage* resize(char* image_name, location* loc);
int getOrientation(int sourceWidth, int sourceHeight, int destWidth, int destHeight);
void setCropROI(IplImage* image,location* loc, int orientation);
IplImage* border(IplImage* image, location* loc, int orientation);
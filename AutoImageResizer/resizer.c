#include "resizer.h"

IplImage* resize(char* image_name, location* loc){
    IplImage* img;
    IplImage* new_img;
    if(!(img=cvLoadImage(image_name,CV_LOAD_IMAGE_COLOR))){
        return NULL;
    }
    if (loc->scale_or_resize == SCALE) {
        int x = img->width * loc->scale_factor;
        int y = img->height * loc->scale_factor;
        new_img = cvCreateImage(cvSize(x,y),img->depth,img->nChannels);
    } else {
        new_img = cvCreateImage(cvSize(loc->width,loc->height),img->depth,img->nChannels);
        if(loc->keep_aspect_ratio){
            int orientation = getOrientation(img->width, img->height, loc->width, loc->height);
            if (orientation == HORIZONTAL) {
                if (loc->resize_horizontal_image_behavior > 3) {
                    setCropROI(img, loc, orientation);
                } else {
                    img = border(img, loc, orientation);
                }
            } else if(orientation == VERTICAL){
                if (loc->resize_vertical_image_behavior > 3) {
                    setCropROI(img, loc, orientation);
                } else {
                    img = border(img, loc, orientation);
                }
            }
        }
    }
    cvResize(img,new_img,CV_INTER_LINEAR);
    cvReleaseImage(&img);
    return new_img;
}

int getOrientation(int sourceWidth, int sourceHeight, int destWidth, int destHeight){
    double sourceRatio = (double)sourceWidth/(double)sourceHeight;
    double destRatio = (double)destWidth/(double)destHeight;
    if (sourceRatio > destRatio) {
        return HORIZONTAL;
    }else if (sourceRatio == destRatio){
        return PROPORTIONAL;
    }
    return VERTICAL;
}

void setCropROI(IplImage* image, location* loc, int orientation){
    CvRect rect;
    int offset;
    double scaleFactor;
    if (orientation == HORIZONTAL) {
        int width = (image->width * loc->height) / image->height;
        scaleFactor = (double) image->width / width;
        width = loc->width * scaleFactor;
        if (loc->resize_horizontal_image_behavior == CROP_LEFT) {
            offset = image->width - width;
        } else if(loc->resize_horizontal_image_behavior == CROP_RIGHT) {
            offset = 0;
        } else {
            offset = (image->width - width)/2;
        }
        rect = cvRect(abs(offset), 0, width, image->height);
    } else {
        int height = (image->height * loc->width) / image->width;
        scaleFactor = (double) image->height / height;
        height = loc->height * scaleFactor;
        if (loc->resize_vertical_image_behavior == CROP_TOP) {
            offset = image->height - height;
        } else if (loc->resize_vertical_image_behavior == CROP_BOTTOM){
            offset = 0;
        } else {
            offset = (image->height - height)/2;
        }
        rect = cvRect(0, abs(offset), image->width, height);
    }
    cvSetImageROI(image, rect);
    //printf("x = %d y = %d width = %d height = %d offset = %d  :%s\n", rect.x,rect.y,rect.width,rect.height,offset,loc->dest_name);
}
IplImage* border(IplImage* image, location* loc, int orientation){
    int offsetX;
    int offsetY;
    int width;
    int height;
    double scaleFactor;
    CvPoint offset;
    IplImage* border;
    if (orientation == VERTICAL) {
        height = image->height;
        width = (image->width * loc->height) / image->height;
        scaleFactor = (double) image->width / width;
        width = loc->width * scaleFactor;
        offsetY = 0;
        if (loc->resize_horizontal_image_behavior == BORDER_TOP) {
            offsetX = 0;
        } else if(loc->resize_horizontal_image_behavior == BORDER_BOTTOM) {
            offsetX = image->width - width;
        } else {
            offsetX = (image->width - width)/2;
        }
    } else {
        width = image->width;
        height = (image->height * loc->width) / image->width;
        scaleFactor = (double) image->height / height;
        height = loc->height * scaleFactor;
        offsetX = 0;
        if (loc->resize_vertical_image_behavior == BORDER_LEFT) {
            offsetY = 0;
        } else if (loc->resize_vertical_image_behavior == BORDER_RIGHT){
            offsetY = image->height - height;
        } else {
            offsetY = (image->height - height)/2;
        }
    }
    offset = cvPoint(abs(offsetX), abs(offsetY));
    border = cvCreateImage(cvSize(width,height),image->depth,image->nChannels);
    cvCopyMakeBorder(image, border, offset, IPL_BORDER_CONSTANT, CV_RGB(loc->border_space_color[0], loc->border_space_color[1], loc->border_space_color[2]));
    cvReleaseImage(&image);
    return border;
}















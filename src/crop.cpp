/*
    This file allows the croping of bounding boxes
*/
#include <iostream>
#include "crop.h"

using namespace std;
using namespace cv;

bool crop (Mat& src,Mat& dst, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    
    if (x+w >= src.cols)
        w= src.cols-x-1;//if we are out of image, width is reduced to reach edge of image
    if (y+h >= src.rows)
        h=src.rows-y-1;//same as width
    dst = src (Rect(x,y,w,h));//crop
    return true;//useless TODO: fix this
}

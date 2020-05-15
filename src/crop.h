#ifndef CROP_H
#define CROP_H
#include <opencv2/opencv.hpp>

bool crop (cv::Mat& src,cv::Mat& dst, unsigned int x, unsigned int y, unsigned int w, unsigned int h);



#endif

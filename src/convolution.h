#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <opencv2/opencv.hpp>



//functions
void help (const char*progName);
void blur (cv::Mat& src,cv::Mat& dst, double **kernel, int size=5);
bool freeKernel (double **kernel,int size);
void gaussKernel(double **GKernel,int size,double sigma) ;
void blurGray (cv::Mat& src,cv::Mat& dst, double** kernel,int size);
void homogeneousKernel (double** kernel, int size);
void optimizedBlur (cv::Mat& src, cv::Mat& dst, double* kernel, int size);
void optimizedBlurGray (cv::Mat& src, cv::Mat& dst, double* kernel, int size);
void gaussKernel1D (double * kernel, int size,double sigma);
void freeKernel1D (double * kernel);

#endif

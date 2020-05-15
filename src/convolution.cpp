/*
This file could be considered optional for now, as i'm using afterall the opencv bluring function unstead of this self implemented functions.
The processing time is the same.
This file contains multiple bluring algorithms for grayscale and rgb images.
The edges arent treated in these algorithms, that's why i'm using opencv blurring function for now
TODO fix edges and use these functions unstead
*/
#include "convolution.h"
#include <math.h>
#include <iostream>

using namespace std;
using namespace cv;

void help (const char *progName)
{
        cout << "This program blurs the detected bounding boxes in a video file." <<endl;
        cout << "USAGE : "<<progName<<" [options]  "<<endl;
        cout << "Options are : "<<endl<<"-i: then specify input video path"<<endl<<"-o: then specify output path"<<endl<<"-f: then specify frames file path"<<endl<<"-b: then specify boxes file path"<<endl<<"-s: then specify sigma's value"<<endl<<endl<<
"Default: "<<endl<<"Input video: ../../Seat detection.avi"<<endl<<"Output video: results/blurred_video.avi"<<endl<<"frames: /home/fluidlabcctv/Mourad/Darknet/darknet_face/frame.txt"<<endl<<"Boxes: /home/fluidlabcctv/Mourad/Darknet/darknet_face/boxes.txt"<<endl<<"Sigma: 100"<<endl<<endl;
//    cout <<"Press any button then press enter to start"<<endl;
//    int test;
//    cin>>test;
    cout<<endl;
//  this_thread::sleep_for(chrono::milliseconds(5000));
}

void blur (Mat& src,Mat& dst, double** kernel,int size)
{

    int kernelCenter=size/2;
    for (int i=0;i<src.rows;i++)
    {
        for (int j=0;j<src.cols;j++)
        {
            Vec3b& out = dst.at<Vec3b>(Point(j,i)); // Reference the input pixel
            Vec3d sum; // summing for convolution (double)
            for (int k=0;k<3;k++)
                sum[k]=0;
            for (int m=0;m<size;m++)
            {
                int mm = size-1-m; //flip 
                for (int n=0; n<size;n++)
                {
                    int nn =size-1-n; //flip

                    int ii=i+kernelCenter - mm; //correct indexing of in input since kernel's indexing is centered
                    int jj=j+kernelCenter -nn; //same
                    if (ii >= 0 && ii < src.rows && jj >= 0 && jj < src.cols)
                    {
                         Vec3b& in = src.at<Vec3b>(Point (jj,ii)); // Reference the output pixel
                         sum[0] += in[0]*kernel[mm][nn];   // convolute for each channel
                         sum[1] += in[1]*kernel[mm][nn];
                         sum[2] += in[2]*kernel[mm][nn];
                    }

                }
            }
            out [0]=(unsigned char)((float)fabs(sum[0])); // convert to unsigned char
            out [1]=(unsigned char)((float)fabs(sum[1]));
            out [2]=(unsigned char)((float)fabs(sum[2]));
        }
    }
}

void blurGray (Mat& src,Mat& dst, double** kernel,int size) // refer to function above for explanations
{
    int kernelCenter=size/2;
    for (int i=0;i<src.rows;i++)
    {
        for (int j=0;j<src.cols;j++)
        {
            uchar& out = dst.at<uchar>(Point(j,i));
            double sum=0;

            for (int m=0;m<size;m++)
            {
                int mm = size-1-m;
                for (int n=0; n<size;n++)
                {
                    int nn =size-1-n;

                    int ii=i+kernelCenter - mm;
                    int jj=j+kernelCenter -nn;
                    if (ii >= 0 && ii < src.rows && jj >= 0 && jj < src.cols)
                    {
                         uchar& in = src.at<uchar>(Point (jj,ii));
                         sum += in*kernel[mm][nn];

                    }

                }
            }
            out =(unsigned char)((float)fabs(sum));
        }
    }
}


bool freeKernel (double **kernel,int size)
{
    for (int i=0;i<size;i++)
        delete [] kernel[i] ;
    delete [] kernel;
    return (kernel==NULL);
}

void gaussKernel(double ** kernel,int size, double sigma)
{

    double r, s = 2.0 * sigma * sigma;
    int half = size/2;

    // sum  for normalization 
    double sum = 0.0;

    // generating kernel 
    for (int x = -1*half; x <= half; x++)
    {
        for (int y = -1*half; y <= half; y++)
        {
            r = sqrt(x * x + y * y);
            kernel[x + half][y + half] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += kernel[x + half][y + half];
        }
    }

    // normalizing the Kernel 
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            kernel[i][j] /= sum;
}

void homogeneousKernel (double** kernel, int size)
{
    double sum=0.0;
    for (int i=0;i<size;i++)
        for (int j=0;j<size;j++)
        {
            kernel[i][j]=1;
            sum++;
        }

    for (int i=0;i<size;i++)
        for (int j=0;j<size;j++)
            kernel[i][j]/=sum;
}

void optimizedBlurGray (Mat& src, Mat& dst, double* kernel, int size)
{
    int kernelCenter=size/2;
    Mat tmp;
    src.copyTo(tmp);

    for (int i=0;i<src.rows;i++)
    {

        for (int j=0;j<src.cols;j++)
        {
            uchar& out= tmp.at<uchar>(Point(j,i));
            double sum=0.0;
            for (int m=0;m<size;m++)
            {
                int mm=size-1-m;
                int ii = i+kernelCenter-mm;
                if (ii >= 0 && ii < src.rows)
                {
                    uchar& in =src.at<uchar>(Point(j,ii));
                    sum+=in*kernel[mm];
                }
            }
            out =(unsigned char)((float)fabs(sum));
        }
    }
    for (int i=0;i<src.rows;i++)
    {

        for (int j=0;j<src.cols;j++)
        {
            uchar& out= dst.at<uchar>(Point(j,i));
            double sum=0.0;
            for (int n=0;n<size;n++)
            {
                int nn=size-1-n;
                int jj = j+kernelCenter-nn;
                if (jj >= 0 && jj < src.cols)
                {
                    uchar& in =tmp.at<uchar>(Point(jj,i));
                    sum+=in*kernel[nn];
                }
            }
            out =(unsigned char)((float)fabs(sum));
        }
    }
    tmp.release();
}

void optimizedBlur (Mat& src, Mat& dst, double* kernel, int size)//Doing two 1D convolutions unstead of 1 2D convolution. Much faster
{
    int kernelCenter=size/2;
    Mat tmp;//temporary for first convolution
    tmp=src;//change directly on sourche file
    
    for (int i=0;i<src.rows;i++)
    {

        for (int j=0;j<src.cols;j++)
        {
            Vec3b& out= tmp.at<Vec3b>(Point(j,i));//access pixel
            Vec3d sum;
            for (int i=0;i<3;i++)
                sum[i]=0.0;
            for (int m=0;m<size;m++)
            {
                int mm=size-1-m;//flip
                int ii = i+kernelCenter-mm;//refer to 2D blurring
                if (ii >= 0 && ii < src.rows)
                {
                    Vec3b& in =src.at<Vec3b>(Point(j,ii));
                    sum[0]+=in[0]*kernel[mm];
                    sum[1]+=in[1]*kernel[mm];
                    sum[2]+=in[2]*kernel[mm];
                }
            }
            out[0] =(unsigned char)((float)fabs(sum[0]));
            out[1] =(unsigned char)((float)fabs(sum[1]));
            out[2] =(unsigned char)((float)fabs(sum[2]));
        }
    }
    dst=tmp;//change directly on source file
    for (int i=0;i<src.rows;i++)
    {

        for (int j=0;j<src.cols;j++)
        {
            Vec3b& out= dst.at<Vec3b>(Point(j,i));
            Vec3d sum;
            for (int i=0;i<3;i++)
                sum[i]=0.0;

            for (int n=0;n<size;n++)
            {
                int nn=size-1-n;
                int jj = j+kernelCenter-nn;
                if (jj >= 0 && jj < src.cols)
                {
                    Vec3b& in =tmp.at<Vec3b>(Point(jj,i));
                    sum[0]+=in[0]*kernel[nn];
                    sum[1]+=in[1]*kernel[nn];
                    sum[2]+=in[2]*kernel[nn];
                }
            }
            out[0] =(unsigned char)((float)fabs(sum[0]));
            out[1] =(unsigned char)((float)fabs(sum[1]));
            out[2] =(unsigned char)((float)fabs(sum[2]));
        }
    }
    tmp.release();
}


void gaussKernel1D (double* kernel, int size, double sigma)
{
    double  s = 2.0 * sigma * sigma;
    int half = size/2;

    // sum  for normalization 
    double sum = 0.0;

    // generating kernel 
    for (int x = -1*half; x <= half; x++)
    {
        kernel[x + half] = (exp(-(x * x) / s)) / (M_PI * sqrt(sigma*2));
        sum += kernel[x + half];
    }


    // normalizing the Kernel 
    for (int i = 0; i < size; ++i)
        kernel[i] /= sum;
}

void freeKernel1D (double * kernel)
{
    delete [] kernel;
}
                                                                  

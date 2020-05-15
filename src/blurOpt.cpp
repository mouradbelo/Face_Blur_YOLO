#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include "convolution.h"
#include "crop.h"
#include <fstream>

using namespace std;
using namespace cv;

#define PATH /home/fluidlabcctv/Mourad/Darknet/darknet_face

int main(int argc, char *argv[])
{
	help(argv[0]);                  // how to use TODO [update it !]
        //const char* path; 
        //double s;                       // sigma's Value*/
        char** endptr = NULL;           // for strtol

        const char *inVidPath=NULL;
        const char *outVidPath=NULL;
        const char *boxesPath=NULL;
        const char *framesPath=NULL;  
        double s=100;
        
        for (int i=1;i<argc;i++)
        {
            if (i+1!= argc)
            {    
                if (!strcmp(argv[i],"-i"))
                    inVidPath=argv[i+1];
                else if (!strcmp(argv[i],"-o"))
                    outVidPath=argv[i+1];
                else if (!strcmp(argv[i],"-b"))
                    boxesPath=argv[i+1];
                else if (!strcmp(argv[i],"-f"))
                    framesPath=argv[i+1];
                else if (!strcmp(argv[i],"-s"))
                    s=strtol(argv[i+1],endptr,10);
                else
                    cout << "Default arguments will be used for none specified arguments"<<endl;
            }
        }
        if (inVidPath==NULL)
            inVidPath="../../Seat detection.avi";
        if (outVidPath==NULL)
            outVidPath="results/blurred_video.avi";
        if (boxesPath==NULL)
            boxesPath="/home/fluidlabcctv/Mourad/Darknet/darknet_face/boxes.txt";
        if (framesPath==NULL)
            framesPath="/home/fluidlabcctv/Mourad/Darknet/darknet_face/frame.txt";
        if (s==0)
            s=100;

            
	//path=argc >1 ? argv[1] : "../../darknet_V3.0/Seat detection.avi";//Video to process
        //s= argc>2 ? strtol(argv[2],endptr,10):100.0;// Sigma*/
    
    
        VideoCapture vid;
        vid.open (inVidPath); 
                
	if (!vid.isOpened())
	{
		cerr<<"ERROR, couldn't load file " << inVidPath <<endl;
		return -1;
	}
        /*cout << vid.get (CV_CAP_PROP_FRAME_WIDTH)<<endl;
        cout << vid.get (CV_CAP_PROP_FRAME_HEIGHT)<<endl;*/
        
        Size S = Size((int) vid.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) vid.get(CV_CAP_PROP_FRAME_HEIGHT)); 
        int ex = static_cast<int>(vid.get(CV_CAP_PROP_FOURCC));// codec
        VideoWriter output;
                   
         output.open(outVidPath, /*CV_FOURCC('X','2','6','4') or MP42,*/ex, /*60*/vid.get(CV_CAP_PROP_FPS), S, true);//video name and path specified by the string, Change FPS if needed
        if (!output.isOpened())
        {
            cout  << "Could not open the output video for write: " << endl;
            return -1;
        }
        
         
        Mat frame;//frame browser
        ifstream f(boxesPath);//file containing the bounding boxes in each frame
        if (!f)
        {
            cerr<<"Couldn't open txt file"<<endl;
            return -1;
        }
        ifstream fr (framesPath);// file containing all the frames in the video (except first frame because it's skipped by yolo. (Tried not skipping it, yolo crashes with cuda error TODO Try again)
        if (!fr)
        {
            cerr<<"Couldn't open txt file"<<endl;
            return -1;
        }

        
        vid>>frame;// skip first frame as it's not processed by yolo  
        vid>>frame;
        string content;// boxes.txt file browser
        string content2;//frame.txt file browser
        fr>>content2;  // initialize with first frame
        
        unsigned int counter=0;
        double time=getTickCount();
        int length=(int)vid.get(CV_CAP_PROP_FRAME_COUNT);
	//cout<<length<<endl;
        cout << "processing"<<endl;        

        int frameId;        //tracking frames in boxes.txt
        int frameIdAll=stoi(content2);//tracking frames in frame.txt to compare.
        bool flag=true;//This flag is of utmost importance, it indicates if this frame has bounding boxes or shouldn't be processed (TODO:change the name )
        unsigned int x(0),y(0),w(0),h(0);//speak for themselves ( top left pixel specified with x and y)
        while ((!f.eof()))// loop through the bounding boxes of each frame
        {   
            int i=1;// used to browse a line in boxes.txt
            if (flag)            // if current line hasen't been processed because of frame skipping, keep current line
                while ((i!=6))
                {
                    switch (i)
                    {
                        case 1:     f>>content;frameId=stoi(content);//save frameId
                                        break;
                        case 2:     f >> content;x=stoi (content);//save top left and so on
                                        break;
                        case 3:     f >> content;y=stoi (content);
                                        break;
                        case 4:     f >> content;w=stoi (content);//width
                                        break;
                        case 5:     f >> content;h=stoi (content);//height 
                                        break;
                        default: cout << "Can't get here anyway !";
                    }
                    i++;
                }
            
           // cout <<frameId<<" " <<x << " "<<y<<" "<<w<<" "<<h<<endl;
            static int lastFrameId=frameId;//Used to track if this bounding box concerns a new frame or if it's for the current frame

            if (lastFrameId!=frameId)// if we get to a new frame, we also increment the frame number in frame.txt
            {
                fr>>content2;
                frameIdAll=stoi(content2);
            } 
                
                
            if (frameIdAll==frameId)// should this frame be treated ? if not equal, it means we need to skip frames until they are equal. These skipped frames dont contain bouding boxes
            {
                //cout <<frameId<<" "<< x<<" "<< y<<" "<<w<<" "<<h<<" "<<endl;
                if (lastFrameId==frameId)//if equal, this is the same frame but a new bounding box, so we only need to blur and not pass the frame to output
                {
                    Mat srcCropped,dst;
                    crop (frame,srcCropped,x,y,w,h); // crop image
            
                    srcCropped.copyTo(dst);// make sure the destination has same size
                    unsigned int size = srcCropped.rows < srcCropped.cols ? srcCropped.rows/2 : srcCropped.cols/2; // size is the smallest between height and width
                    if (size%2==0) // size needs to be odd for convolution
                    size--;
                /*double* gaussKernel = new double [size]; // 1D gaussian kernel
                gaussKernel1D (gaussKernel, size, s);

                optimizedBlur (srcCropped,dst,gaussKernel,size);
                
                freeKernel1D(gaussKernel);*/
                    dst=srcCropped;
                    GaussianBlur (srcCropped,dst,Size(size,size),s,s);//blur the cropped bounding box
                    srcCropped.release();
                    dst.release();
                }
                else// go to next frame as all the lines (bounding boxes) concerning this frame has been treated
                {    
                    output<<frame;  //save last frame as it has been completely treated
                    counter++;
	    	    if (floor(((float)counter/length)*100)==((float)counter/length)*100)
                    	cout<<((float)counter/length)*100<<"%"<<endl;//print % of processing
                    /*imshow ("result",frame);
                    waitKey(0);*/
                    vid>>frame;//go to next frame
            
                    // the current bounding box concerns this new frame, let's process it (else we'd skip this bounding box)
                    Mat srcCropped,dst;
                    crop (frame,srcCropped,x,y,w,h); // crop image
            
                    srcCropped.copyTo(dst);// make sure the destination has same size
                    unsigned int size = srcCropped.rows < srcCropped.cols ? srcCropped.rows/2 : srcCropped.cols/2; // size is the smallest between height and width
                    if (size%2==0) // size needs to be odd for convolution, check convolution doc
                    size--;   
                    /*   double* gaussKernel = new double [size]; // 1D gaussian kernel
                    gaussKernel1D (gaussKernel, size, s);

            
                    optimizedBlur (srcCropped,dst,gaussKernel,size);             
            
                    freeKernel1D(gaussKernel); */
                    dst=srcCropped;
                    GaussianBlur (srcCropped,dst,Size(size,size),s,s);

                    srcCropped.release();
                    dst.release();
                }
                lastFrameId=frameId;//save the last frame we processed
                flag=true;            // set the flag to true as we've done processing on this frame
            }
            
            else // this frame doesn't have any bounding boxes
            {
                output<<frame;//save the frame
                /*imshow("result",frame);
                waitKey(0);*/
                counter++;
	    	if (floor(((float)counter/length)*100)==((float)counter/length)*100)
                    cout<<((float)counter/length)*100<<"%"<<endl;
                if (lastFrameId==frameId)
                {
                    fr>>content2;
                    frameIdAll=stoi(content2);
                }
                vid>>frame;//go to next frame
                flag=false;//specify that no treatment has been done on this frame
            }  
        }
        while (!fr.eof())//browse the remaining frames in the video if any and save them (no treatment needed for them)
        {
            vid>>frame;
            fr>>content2;
            output<<frame;
            counter++;
	   if (floor(((float)counter/length)*100)==((float)counter/length)*100)
            	cout<<((float)counter/length)*100<<"%"<<endl;
        }
        cout<<endl<<"processing time: " <<(getTickCount()-time)/getTickFrequency()<<" s"<<endl;
        //releasing
        frame.release();
        f.close();	
        fr.close();
        vid.release();
        output.release();
        cout <<endl<<"Success"<<endl;
	return 0;
}



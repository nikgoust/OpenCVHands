#ifndef _MYIMAGE_
#define _MYIMAGE_ 

#include<opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

class MyImage{
	public:
		MyImage(int webCamera);
		Mat srcLR;
		Mat src_frame;
		Mat bw;
		vector<Mat> bwList;
		VideoCapture video_capture;	
		
};



#endif

#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "myImage.hpp"
#include "roi.hpp"
#include "handGesture.hpp"
#include <vector>
#include "main.hpp"

using namespace cv;
using namespace std;

/* Global Variables  */
int Square_len = 20;
int AvgColor[NSAMPLES][3] ;
int C_lower[NSAMPLES][3];
int C_upper[NSAMPLES][3];
vector <MyROI> roi;
MyImage *my_image;
HandGesture hand_gesture;

int getMedian(vector<int> val) {
	int median;
	size_t size = val.size();
	sort(val.begin(), val.end());
	if (size % 2 == 0) {
		median = val[size / 2 - 1];
	}
	else {
		median = val[size / 2];
	}
	return median;
}

void getAvgColor(MyROI roi, int avg[3]) {
	Mat r;
	roi.roi_ptr.copyTo(r);
	vector<int>hm;
	vector<int>sm;
	vector<int>lm;
	for (int i = 2; i<r.rows - 2; i++) {
		for (int j = 2; j<r.cols - 2; j++) {
			hm.push_back(r.data[r.channels()*(r.cols*i + j) + 0]);
			sm.push_back(r.data[r.channels()*(r.cols*i + j) + 1]);
			lm.push_back(r.data[r.channels()*(r.cols*i + j) + 2]);
		}
	}
	avg[0] = getMedian(hm);
	avg[1] = getMedian(sm);
	avg[2] = getMedian(lm);
}

void drawRectangleAndWait() {
	my_image->video_capture >> my_image->src_frame;
	flip(my_image->src_frame, my_image->src_frame, 1);
	roi.push_back(MyROI(Point(my_image->src_frame.cols / 3, my_image->src_frame.rows / 6), Point(my_image->src_frame.cols / 3 + Square_len, my_image->src_frame.rows / 6 + Square_len), my_image->src_frame));
	roi.push_back(MyROI(Point(my_image->src_frame.cols / 4, my_image->src_frame.rows / 2), Point(my_image->src_frame.cols / 4 + Square_len, my_image->src_frame.rows / 2 + Square_len), my_image->src_frame));
	roi.push_back(MyROI(Point(my_image->src_frame.cols / 3, my_image->src_frame.rows / 1.5), Point(my_image->src_frame.cols / 3 + Square_len, my_image->src_frame.rows / 1.5 + Square_len), my_image->src_frame));
	roi.push_back(MyROI(Point(my_image->src_frame.cols / 2, my_image->src_frame.rows / 2), Point(my_image->src_frame.cols / 2 + Square_len, my_image->src_frame.rows / 2 + Square_len), my_image->src_frame));
	roi.push_back(MyROI(Point(my_image->src_frame.cols / 2.5, my_image->src_frame.rows / 2.5), Point(my_image->src_frame.cols / 2.5 + Square_len, my_image->src_frame.rows / 2.5 + Square_len), my_image->src_frame));
	roi.push_back(MyROI(Point(my_image->src_frame.cols / 2, my_image->src_frame.rows / 1.5), Point(my_image->src_frame.cols / 2 + Square_len, my_image->src_frame.rows / 1.5 + Square_len), my_image->src_frame));
	roi.push_back(MyROI(Point(my_image->src_frame.cols / 2.5, my_image->src_frame.rows / 1.8), Point(my_image->src_frame.cols / 2.5 + Square_len, my_image->src_frame.rows / 1.8 + Square_len), my_image->src_frame));


	for (int i = 0; i < 50; i++) {
		my_image->video_capture >> my_image->src_frame;
		flip(my_image->src_frame, my_image->src_frame, 1);
		for (int j = 0; j < NSAMPLES; j++) {
			roi[j].draw_rectangle(my_image->src_frame);
		}
		imshow("img1", my_image->src_frame);
		if (cv::waitKey(30) >= 0) break;
	}
}


void getHandColor(){	
		my_image-> video_capture >> my_image-> src_frame;
		flip(my_image-> src_frame, my_image-> src_frame, 1);
		cvtColor(my_image-> src_frame, my_image-> src_frame, CV_BGR2HLS);
		for(int j = 0; j < NSAMPLES; j++){
			getAvgColor(roi[j], AvgColor[j]);
			roi[j].draw_rectangle(my_image-> src_frame);
		}	
		cvtColor(my_image-> src_frame, my_image-> src_frame, CV_HLS2BGR);
		imshow("img1", my_image-> src_frame);	
}

void normalizeColors(){
	// copy all boundries read from trackbar
	// to all of the different boundries
	for(int i = 1; i < NSAMPLES; i++){
		for(int j = 0; j < 3; j++){
			C_lower[i][j] = C_lower[0][j];	
			C_upper[i][j] = C_upper[0][j];	
		}	
	}
	// normalize all boundries so that 
	// threshold is whithin 0-255
	for(int i=0;i<NSAMPLES;i++){
		if((AvgColor[i][0]-C_lower[i][0]) <0){
			C_lower[i][0] = AvgColor[i][0] ;
		}if((AvgColor[i][1]-C_lower[i][1]) <0){
			C_lower[i][1] = AvgColor[i][1] ;
		}if((AvgColor[i][2]-C_lower[i][2]) <0){
			C_lower[i][2] = AvgColor[i][2] ;
		}if((AvgColor[i][0]+C_upper[i][0]) >255){ 
			C_upper[i][0] = 255-AvgColor[i][0] ;
		}if((AvgColor[i][1]+C_upper[i][1]) >255){
			C_upper[i][1] = 255-AvgColor[i][1] ;
		}if((AvgColor[i][2]+C_upper[i][2]) >255){
			C_upper[i][2] = 255-AvgColor[i][2] ;
		}
	}
}

void produceBinaries(){	
	Scalar lowerBound;
	Scalar upperBound;
	Mat foo;
	for(int i=0;i<NSAMPLES;i++){
		normalizeColors();
		lowerBound=Scalar(AvgColor[i][0] - C_lower[i][0], AvgColor[i][1] - C_lower[i][1], AvgColor[i][2] - C_lower[i][2]);
		upperBound=Scalar(AvgColor[i][0] + C_upper[i][0], AvgColor[i][1] + C_upper[i][1], AvgColor[i][2] + C_upper[i][2]);
		my_image-> bwList.push_back(Mat(my_image-> srcLR.rows,my_image-> srcLR.cols, CV_8U));	
		inRange(my_image-> srcLR,lowerBound,upperBound,my_image-> bwList[i]);	
	}
	my_image-> bwList[0].copyTo(my_image-> bw);
	for(int i=1;i<NSAMPLES;i++){
		my_image-> bw+=my_image-> bwList[i];	
	}
	medianBlur(my_image->  bw, my_image->  bw,7);
}

void showWindows(){
	pyrDown(my_image-> bw, my_image-> bw);
	pyrDown(my_image-> bw, my_image-> bw);
	Rect roi( Point( 3* my_image->src_frame.cols/4,0 ), my_image->bw.size());
	vector<Mat> channels;
	Mat result;
	channels.reserve(3);
	for(int i=0;i<3;i++)
		channels.push_back(my_image->bw);
	merge(channels,result);
	result.copyTo(my_image->src_frame(roi));
	imshow("img1", my_image->src_frame);
}

int findBiggestContour(vector<vector<Point> > contours){
    int indexOfBiggestContour = -1;
    int sizeOfBiggestContour = 0;
    for (int i = 0; i < contours.size(); i++){
        if(contours[i].size() > sizeOfBiggestContour){
            sizeOfBiggestContour = contours[i].size();
            indexOfBiggestContour = i;
        }
    }
    return indexOfBiggestContour;
}

void myDrawContours()
{
	drawContours(my_image->src_frame,hand_gesture.hullP,hand_gesture.cIdx,cv::Scalar(200,0,0),2, 8, vector<Vec4i>(), 0, Point());
	rectangle(my_image->src_frame,hand_gesture.bRect.tl(),hand_gesture.bRect.br(),Scalar(0,0,200));	
	vector<Mat> channels;
	Mat result;
	channels.reserve(3);
	for(int i = 0; i < 3; i++)
			channels.push_back(my_image-> bw);
	merge(channels, result);
	drawContours(result, hand_gesture.hullP, hand_gesture.cIdx, cv::Scalar(0,0,250), 10, 8, vector<Vec4i>(), 0, Point());

	if (hand_gesture.cIdx < 0)return;
	vector<Vec4i>::iterator d = hand_gesture.defects[hand_gesture.cIdx].begin();
		
	while( d != hand_gesture.defects[hand_gesture.cIdx].end()) 
	{
   	    Vec4i& v=(*d);
	    int startidx=v[0]; 
		Point ptStart(hand_gesture.contours[hand_gesture.cIdx][startidx] );
   		int endidx=v[1]; 
		Point ptEnd(hand_gesture.contours[hand_gesture.cIdx][endidx]);
  	    int faridx=v[2]; 
		Point ptFar(hand_gesture.contours[hand_gesture.cIdx][faridx] );
	    float depth = v[3] / 256;
   		circle( result, ptFar,   9, Scalar(0,205,0), 5 );				
	    d++;
   	 }

}

void makeContours(HandGesture* hg){
	Mat aBw;
	pyrUp(my_image-> bw,my_image-> bw);
	my_image-> bw.copyTo(aBw);
	findContours(aBw,hg->contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	hg->initVectors(); 
	hg->cIdx = findBiggestContour(hg->contours);
	if(hg->cIdx!=-1){
		hg->bRect=boundingRect(Mat(hg->contours[hg->cIdx]));		
		convexHull(Mat(hg->contours[hg->cIdx]),hg->hullP[hg->cIdx],false,true);
		convexHull(Mat(hg->contours[hg->cIdx]),hg->hullI[hg->cIdx],false,false);
		approxPolyDP( Mat(hg->hullP[hg->cIdx]), hg->hullP[hg->cIdx], 18, true );
		if(hg->contours[hg->cIdx].size()>3 ){
			convexityDefects(hg->contours[hg->cIdx],hg->hullI[hg->cIdx],hg->defects[hg->cIdx]);
			hg->eleminateDefects();
		}		
	}
}

int init()
{
	for (int i = 0; i < NSAMPLES; i++)
	{
		C_lower[i][0] = 12;
		C_upper[i][0] = 7;
		C_lower[i][1] = 30;
		C_upper[i][1] = 40;
		C_lower[i][2] = 80;
		C_upper[i][2] = 80;
	}
	my_image = new MyImage(0);
	my_image-> video_capture >> my_image-> src_frame;
	namedWindow("img1", CV_WINDOW_KEEPRATIO);
	drawRectangleAndWait();
	getHandColor();
	destroyWindow("img1");
	namedWindow("img1", CV_WINDOW_FULLSCREEN);	
	return 0;
}

void tmpmain()
{	
	my_image-> video_capture >> my_image-> src_frame;
	flip(my_image-> src_frame, my_image-> src_frame, 1);
	pyrDown(my_image-> src_frame, my_image-> srcLR);
	blur(my_image-> srcLR, my_image-> srcLR, Size(3, 3));
	cvtColor(my_image-> srcLR, my_image-> srcLR, CV_BGR2HLS);
	produceBinaries();
	cvtColor(my_image-> srcLR, my_image-> srcLR, CV_HLS2BGR);
	makeContours(&hand_gesture);

	hand_gesture.detectIfHand();
	hand_gesture.printGestureInfo(my_image->src_frame);
	if (hand_gesture.isHand) {
		hand_gesture.getFingerTips(my_image);
		hand_gesture.drawFingerTips(my_image);
		myDrawContours();
	}
	hand_gesture.getFingerNumber(my_image);
	showWindows();
}


int main()
{
	init();
	for (;;) 
	{
		tmpmain();
		if (waitKey(30) == char('q')) break;
	}
	destroyAllWindows();
	my_image-> video_capture.release();
    return 0;
}


extern "C" void __declspec(dllexport) __stdcall  Close()
{
	my_image->video_capture.release();
	destroyAllWindows();
	roi.clear();
}

extern "C" int __declspec(dllexport) __stdcall  Init(int video_source, int& outCameraWidth, int& outCameraHeight)
{
	my_image = new MyImage(video_source);

	if (!my_image->video_capture.isOpened())
		return -1;

	for (int i = 0; i < NSAMPLES; i++)
	{
		C_lower[i][0] = 12;
		C_upper[i][0] = 7;
		C_lower[i][1] = 30;
		C_upper[i][1] = 40;
		C_lower[i][2] = 80;
		C_upper[i][2] = 80;
	}
	outCameraWidth = my_image->video_capture.get(CAP_PROP_FRAME_WIDTH);
	outCameraHeight = my_image->video_capture.get(CAP_PROP_FRAME_HEIGHT);
	return 0;
}

extern "C" void __declspec(dllexport) __stdcall  SetHandsColor()
{
	/*--------------------DEBUG ZONE START--------------------*/
	namedWindow("img1", CV_WINDOW_KEEPRATIO);
	drawRectangleAndWait();
	destroyWindow("img1");
	/*--------------------DEBUG ZONE END--------------------*/
	my_image->video_capture >> my_image->src_frame;
	flip(my_image->src_frame, my_image->src_frame, 1);
	cvtColor(my_image->src_frame, my_image->src_frame, CV_BGR2HLS);
	for (int j = 0; j < NSAMPLES; j++)
	{
		getAvgColor(roi[j], AvgColor[j]);
	}
}

extern "C" void __declspec(dllexport) __stdcall  GetCoordinates(int& x, int& y)
{
	my_image->video_capture >> my_image->src_frame;
	flip(my_image->src_frame, my_image->src_frame, 1);
	pyrDown(my_image->src_frame, my_image->srcLR);
	blur(my_image->srcLR, my_image->srcLR, Size(3, 3));
	cvtColor(my_image->srcLR, my_image->srcLR, CV_BGR2HLS);
	produceBinaries();
	cvtColor(my_image->srcLR, my_image->srcLR, CV_HLS2BGR);
	makeContours(&hand_gesture);
	//hand_gesture.removeRedundantFingerTips();

	hand_gesture.detectIfHand();
	hand_gesture.printGestureInfo(my_image->src_frame);
	if (hand_gesture.isHand) {
		hand_gesture.getFingerTips(my_image);
		hand_gesture.drawFingerTips(my_image);
		myDrawContours();
	}
	hand_gesture.getFingerNumber(my_image);	
	x = hand_gesture.bRect.x + hand_gesture.bRect_width / 2;
	y = hand_gesture.bRect.y - hand_gesture.bRect_height / 2;
	showWindows();				   //for debug
}


extern "C" int __declspec(dllexport) __stdcall  GetFingersCount()
{		
	if (!hand_gesture.isHand)
	{
		return -1;
	}
	return hand_gesture.fingerTips.size();	
}

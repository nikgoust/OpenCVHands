#ifndef _HAND_GESTURE_
#define _HAND_GESTURE_ 

#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "main.hpp"
#include "myImage.hpp"

using namespace cv;
using namespace std;

class HandGesture{
	public:		
		HandGesture();
		vector<vector<Point> > contours;
		vector<vector<int> >hullI;
		vector<vector<Point> >hullP;
		vector<vector<Vec4i> > defects;	
		vector <Point> fingerTips;
		void printGestureInfo(Mat src);
		int cIdx;
		int frameNumber;
		int mostFrequentFingerNumber;
		int nrOfDefects;
		Rect bRect;
		double bRect_width;
		double bRect_height;
		bool isHand;
		bool detectIfHand();
		void initVectors();
		void getFingerNumber(MyImage *m);
		void eleminateDefects();
		void getFingerTips(MyImage *m);
		void drawFingerTips(MyImage *m);
		void removeRedundantFingerTips();
	private:
		string bool2string(bool tf);
		int fontFace;
		void checkForOneFinger(MyImage *m);
		float getAngle(Point s,Point f,Point e);	
		vector<int> fingerNumbers;
		void analyzeContours();
		string intToString(int number);
		void computeFingerNumber();
		void addNumberToImg(MyImage *m);
		vector<int> numbers2Display;
		void addFingerNumberToVector();
		Scalar numberColor;
		int nrNoFinger;
		float distanceP2P(Point a,Point b);
		void removeRedundantEndPoints(vector<Vec4i> newDefects);
};




#endif

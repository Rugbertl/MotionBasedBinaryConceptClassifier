#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

void samplePoints(Mat gray, vector<Point2f> currFeatures);

int main1(int argc, char** argv)
{
	// use web cam for testing purpose
	VideoCapture capture(0);

	if (!capture.isOpened())
	{
		cout << "error at opening camera" << endl;
		return 1;
	}

	vector<Point2f> currFeatures;
	vector<Point2f> prevFeatures;
	
	int count = 0;
	Mat frame, gray, prevFrame;
	
	while (capture.read(frame))
	{
		// convert to grayscale (for optical flow estimation)
		cvtColor(frame, gray, CV_RGB2GRAY);
		
		// Obtain initial set of features
		//goodFeaturesToTrack(gray, currFeatures, 100, 0.01, 10);
		
		//cv::goodFeaturesToTrack(image_next, // the image 
		//	features_next,   // the output detected features
		//	max_count,  // the maximum number of features 
		//	qlevel,     // quality level
		//	minDist     // min distance between two features
		//	);

		//perform dense sampling of points)
		// TODO: 
		//samplePoints(gray, currFeatures);

		// optical flow estimation
		vector<uchar> status;
		vector<float> err;
		
		if (prevFeatures.size() == currFeatures.size())
		{
			calcOpticalFlowPyrLK(prevFrame, gray, prevFeatures, currFeatures, status, err);
		}

		// visualize result:
		for (int i = 0; i < status.size(); i++)
		{
			// point could be found
			if (status[i] != 0)
			{ 
				Point pt1(ceil(prevFeatures[i].x), ceil(prevFeatures[i].y));
				Point pt2(ceil(currFeatures[i].x), ceil(currFeatures[i].y));
				line(frame, pt1, pt2, Scalar(255, 255, 255));
			}
		}

		imshow("Frame", frame);

		// next time continue with current features
		prevFeatures = currFeatures;
		prevFrame = gray.clone();

		// key handling (reset points)
		int key = waitKey(5);

		if (key == 32)
		{
			prevFeatures.clear();  // force new sampling in next step
		}
	}

	// close VideoCapture etc.
	capture.release();
}


void samplePoints(Mat gray, vector<Point2f> currFeatures)
{
	for (int x = 0; x < gray.cols; x++)
	{
		for (int y = 0; y < gray.rows; y++)
		{
			if (x % 4 == 0 && y % 4 == 0)
			{
				Point2f point = Point(x, y);
				currFeatures.push_back(point);
			}
		}
	}
}
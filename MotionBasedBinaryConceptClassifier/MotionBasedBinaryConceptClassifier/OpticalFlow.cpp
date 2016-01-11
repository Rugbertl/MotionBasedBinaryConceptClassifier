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

vector<Point2f> samplePoints(Mat gray);

int main(int argc, char** argv)
{
	string filename = "C:/Users/peter/Study/Semester7/VideoRetrieval/project/oberstdorf08small.mp4";

	VideoCapture capture(filename);

	if (!capture.isOpened())
	{
		cout << "error at opening video" << endl;
		return 1;
	}

	vector<Point2f> currFeatures;
	vector<Point2f> prevFeatures;

	int count = 0;
	Mat fullframe, frame, gray, prevFrame;

	while (capture.read(fullframe))
	{
		//resize for sake of better demonstration
		//resize(fullframe, frame, Size(fullframe.cols / 2, fullframe.rows / 2));
		
		//convert to grayscale (for optical flow estimation)
		cvtColor(fullframe, gray, CV_RGB2GRAY);

		//perform dense sampling of points)
		currFeatures = samplePoints(gray);

		//Obtain initial set of features
		//goodFeaturesToTrack(gray, currFeatures, 100, 0.01, 20);

		//cv::goodFeaturesToTrack(image_next, // the image 
		//	features_next,   // the output detected features
		//	max_count,  // the maximum number of features 
		//	qlevel,     // quality level
		//	minDist     // min distance between two features
		//	);

		//optical flow estimation
		vector<uchar>  status;
		vector<float>  err;

		if (prevFeatures.size() == currFeatures.size())
		{
			calcOpticalFlowPyrLK(prevFrame, gray, prevFeatures, currFeatures, status, err);
		}

		// visualize  result:
		for (int i = 0; i < status.size(); i++)
		{
			if (status[i] != 0) { //point  could  be found
				Point pt1(ceil(prevFeatures[i].x), ceil(prevFeatures[i].y));
				Point pt2(ceil(currFeatures[i].x), ceil(currFeatures[i].y));
				line(frame, pt1, pt2, Scalar(255, 255, 255));
			}
		}

		//imshow("Frame", frame);
		imshow("Frame", fullframe);

		//next time  continue with current features
		prevFeatures = currFeatures;
		prevFrame = gray.clone();

		//key handling (reset points)
		int key = waitKey(5);

		if (key == 32)
		{
			prevFeatures.clear();  //force  new sampling  in next  step
		}
	}

	// close VideoCapture etc.
	capture.release();
}

vector<Point2f> samplePoints(Mat gray)
{
	vector<Point2f> currFeatures;

	for (int x = 0; x < gray.cols; x++)
	{
		for (int y = 0; y < gray.rows; y++)
		{
			if (x % 4 == 0 && y % 4 == 0)
			{
				Point2f point = Point2f(x, y);
				currFeatures.push_back(point);
			}
		}
	}

	return currFeatures;
}
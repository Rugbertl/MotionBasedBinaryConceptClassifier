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
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;

int main2(int argc, char** argv)
{
	VideoCapture capture(0);

	if (!capture.isOpened())
	{
		cout << "error at opening camera" << endl;
		return 1;
	}

	Ptr<Feature2D> orb = ORB::create();
	Mat frame, gray;

	while (capture.read(frame))
	{
		vector<KeyPoint> kpORB;
		
		// convert to grayscale (for optical flow estimation)
		cvtColor(frame, gray, CV_RGB2GRAY);

		orb->detect(gray, kpORB);

		Mat outputORB;
		drawKeypoints(frame, kpORB, outputORB);
		imshow("Keypoints ORB", gray);
	}

	capture.release();
	return 0;
}
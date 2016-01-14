#include "stdafx.h"
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\video.hpp>
#include <opencv2\imgproc.hpp>
#include <iterator>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;
using namespace cv;

// prototypes
vector<string> GetGroundTruthValues(string filePath);
void VisulizeGroundTruthValues(vector<string> values);
void VisulizeOpticalFlow(Mat &frame, vector<Point2f> prevFeatures, vector<Point2f> currFeatures, vector<uchar> status);
void VisulizeHistogramValues(int histogram[]);
void AddValuesToHistogram(int histogramValues[], vector<Point2f> prevFeatures, vector<Point2f> currFeatures, vector<uchar> status);
int CalculateHistogramIndex(float degree);
vector<string> &split(const string &s, char delim, vector<string> &elems);

string const DEFAULT_PATH1 = "C:/Users/peter/Study/Semester7/VideoRetrieval/project/GroundTruth.csv";
string const DEFAULT_PATH2 = "C:/Users/peter/Study/Semester7/VideoRetrieval/project/oberstdorf08small.mp4";
long double const M_PIl = 3.141592653589793238462643383279502884;
int const NR_OF_BINS = 13;

int main(int argc, char **argv)
{
	// preparation
	vector<string> groundTruthValues;
	// check if a user has specified a non default ground truth
	if (argc > 1)
		groundTruthValues = GetGroundTruthValues(argv[1]);
	else
		groundTruthValues = GetGroundTruthValues(DEFAULT_PATH1);

	VideoCapture capture;
	
	if (argc > 2)
		capture = VideoCapture(argv[2]);
	else
		capture = VideoCapture(DEFAULT_PATH2);

	// check if video could be opend
	if (!capture.isOpened())
	{
		cout << "error at opening video" << endl;
		return 1;
	}
	
	// 13 diffenernt sectors
	int histogramValues[NR_OF_BINS];
	vector<int*> histograms;

	vector<Point2f> currFeatures;
	vector<Point2f> prevFeatures;

	int frameCount = 1;
	int lineCount = 0;				// position in vector groundtruthValues
	bool nextShot = true;
	int shotEnd = -1;

	// parameters for sampling
	int maxCorners = 400;			// maximum number of points returned
	double qualityLevel = 0.01;
	double minDistance = 10.;		// minDistance – The minimum possible Euclidean distance between the returned corners
	int blockSize = 3;

	Mat fullframe, frame, gray, prevFrame;

	while (capture.read(fullframe))
	{
		//resize for sake of better demonstration
		resize(fullframe, frame, Size(fullframe.cols / 1, fullframe.rows / 1));

		//convert to grayscale (for optical flow estimation)
		cvtColor(frame, gray, CV_RGB2GRAY);

		Mat mask;
		Mat cloned = gray.clone();
		
		// perform sampling of keypoints
		goodFeaturesToTrack(cloned, currFeatures, maxCorners, qualityLevel, minDistance);

		// for testing only
		//cout << "** Number of corners detected: " << currFeatures.size() << endl;

		//optical flow estimation
		vector<uchar> status;
		vector<float> err;

		if (prevFeatures.size() == currFeatures.size())
			calcOpticalFlowPyrLK(prevFrame, gray, prevFeatures, currFeatures, status, err);

		// for testing purposes - visualize result
		VisulizeOpticalFlow(frame, prevFeatures, currFeatures, status);
		
		// check if a new shot was found
		if (nextShot)
		{
			cout << "new shot - create histogram";
			
			// remove values from array
			for (int i = 0; i < NR_OF_BINS; i++)
				histogramValues[i] = 0;

			// Shotbegin;Shotend;JumpOff
			vector<string> groundTruthElements;								//TODO: optimize with array of size 3
			split(groundTruthValues[lineCount], ';', groundTruthElements);
			shotEnd = stoi(groundTruthElements[1]);
			nextShot = false;
		}

		// check if next shot started
		if (frameCount == shotEnd)
		{
			int temp[13] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
			AddValuesToHistogram(histogramValues, prevFeatures, currFeatures, status);
			copy(begin(histogramValues), end(histogramValues), begin(temp));
			
			// for testing only
			VisulizeHistogramValues(temp);
			histograms.push_back(temp);
			lineCount++;
			nextShot = true;
		}
		else
		{
			AddValuesToHistogram(histogramValues, prevFeatures, currFeatures, status);
		}

		imshow("Frame", frame);

		//next time continue with current features
		prevFeatures = currFeatures;
		prevFrame = gray.clone();

		//key handling (reset points)
		int key = waitKey(1);

		if (key == 32)
			prevFeatures.clear(); //force new sampling in next step

		frameCount++;
		//cout << frameCount << endl;
	}

	//close VideoCapture etc.
	capture.release();

	return 0;
}

void VisulizeOpticalFlow(Mat &frame, vector<Point2f> prevFeatures, vector<Point2f> currFeatures, vector<uchar> status)
{
	for (int i = 0; i < status.size(); i++)
	{
		if (status[i] != 0)
		{
			//point could be found
			Point pt1(ceil(prevFeatures[i].x), ceil(prevFeatures[i].y));
			Point pt2(ceil(currFeatures[i].x), ceil(currFeatures[i].y));
			line(frame, pt1, pt2, Scalar(255, 255, 255));
		}
	}
}

void VisulizeHistogramValues(int histogram[])
{
	cout << "\t[ ";
	
	for (int i = 0; i < NR_OF_BINS; i++)
		cout << to_string(histogram[i]) << " ";

	cout << "]" << endl;
}

vector<string> &split(const string &s, char delim, vector<string> &elems)
{
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	return elems;
}

// File has to be in a certain format:
//		Shotbegin;Shotend;JumpOff
//		1;1431;0
vector<string> GetGroundTruthValues(string filePath)
{
	vector<string> groundTruthValues;
	ifstream infile(filePath);
	string line;

	// first line is ignored because it contains just the attribute names: Shotbegin Shotend JumpOff
	getline(infile, line);

	while (getline(infile, line))
	{
		istringstream iss(line);
		groundTruthValues.push_back(line);
	}

	return groundTruthValues;
}

void VisulizeGroundTruthValues(vector<string> values)
{
	for each (string s in values)
		cout << s << endl;
}

void AddValuesToHistogram(int histogramValues[], vector<Point2f> prevFeatures, vector<Point2f> currFeatures, vector<uchar> status)
{
	for (int i = 0; i < status.size(); i++)
	{
		if (status[i] != 0)
		{
			//point could be found
			Point pt1(ceil(prevFeatures[i].x), ceil(prevFeatures[i].y));
			Point pt2(ceil(currFeatures[i].x), ceil(currFeatures[i].y));
		
			float angle = atan2(pt1.y - pt2.y, pt1.x - pt2.x);
			float degree = angle * 180 / M_PIl;
		
			int index = CalculateHistogramIndex(degree);

			histogramValues[index]++;
		}
	}
}

// distinguish in this example between 13 directions including non-direction
int CalculateHistogramIndex(float degree)
{
	// optimze here
	degree = round(degree);

	if (degree == 0)
		return 0;
	else if (degree <= 15 && degree > 345 && degree < 360) // exclusive [15-0) (359-344]
		return 1;
	else if (degree <= 345 && degree > 315)
		return 2;
	else if (degree <= 315 && degree > 285)
		return 3;
	else if (degree <= 285 && degree > 255)
		return 4;
	else if (degree <= 255 && degree > 225)
		return 5;
	else if (degree <= 225 && degree > 195)
		return 6;
	else if (degree <= 195 && degree > 165)
		return 7;
	else if (degree <= 165 && degree > 135)
		return 8;
	else if (degree <= 135 && degree > 105)
		return 9;
	else if (degree <= 105 && degree > 75)
		return 10;
	else if (degree <= 75 && degree > 45)
		return 11;
	else if (degree <= 45 && degree > 15)
		return 12;
	else
		return -1;
}
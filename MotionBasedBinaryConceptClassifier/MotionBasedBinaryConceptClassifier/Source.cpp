#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

// prototypes
vector<string> GetGroundTruthValues(string filePath);
void PrintGroundTruthValues(vector<string> values);
void CreateMotionHistorgram();

string DEFAULT_PATH = "C:/Users/peter/Study/Semester7/VideoRetrieval/project/GroundTruth.csv";

int main(int argc, char** argv)
{
	vector<string> groundTruthValues;

	// check if a user has specified a non default ground truth
	if (argc > 1)
	{
		groundTruthValues = GetGroundTruthValues(argv[1]);
	}
	else
	{
		groundTruthValues = GetGroundTruthValues(DEFAULT_PATH);
	}

	PrintGroundTruthValues(groundTruthValues);

	return 0;
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

void PrintGroundTruthValues(vector<string> values)
{
	for each (string s in values)
	{
		cout << s << endl;
	}
}

void CreateMotionHistorgram()
{
	float range[] = { 0, 359 };			// value range
	const float *histRange = { range };

	Mat historgram;

	int value = 2;

	// distinguish in this example between 13 directions including non-direction

	if (value == 0)
	{

	}
	else if (value <= 15 && value > 345 && value < 360) // exclusive [15-1] [359-344]
	{

	}
	else if (value <= 345 && value > 315)
	{

	}
	else if (value <= 315 && value > 285)
	{

	}
	else if (value <= 285 && value > 255)
	{

	}
	else if (value <= 255 && value > 225)
	{

	}
	else if (value <= 225 && value > 195)
	{

	}
	else if (value <= 195 && value > 165)
	{

	}
	else if (value <= 165 && value > 135)
	{

	}
	else if (value <= 135 && value > 105)
	{

	}
	else if (value <= 105 && value > 75)
	{

	}
	else if (value <= 75 && value > 45)
	{

	}
	else if (value <= 45 && value > 15)
	{

	}


}

Mat calculateHistogram(Mat& image, int histSize)
{
	float range[] = { 0, 256 };			// value range
	const float *histRange = { range };

	Mat hist;
	calcHist(&image, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

	return hist;
}
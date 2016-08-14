#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <sstream>
#include <list>
#include "Color.cpp"

using namespace std;
using namespace cv;

/*
	Function to determine the color difference between two pixel points 
	This implements CIEDE2000 algorithm to find the difference in color
*/
bool colorDistance(Vec3b a, Vec3b b, int threshold)
{
	std::vector<double> ca, cb;

	ca.push_back((double)a[2]);
	ca.push_back((double)a[1]);
	ca.push_back((double)a[0]);

	cb.push_back((double)b[2]);
	cb.push_back((double)b[1]);
	cb.push_back((double)b[0]);

	float dist = kallaballa::ciede2000_distance(ca, cb);

	if((int)dist <= threshold)
		return true;
	else
		return false;
}

/*
	Function to modify pixel values at a point as per seed pixel
*/
void modifyPixel(Mat input, int x, int y, int colorflag)
{
	Vec3b &colorC = input.at<Vec3b>(x, y);

	//Yellow Pixel
	if(colorflag == 1)
	{
		colorC[0] = 0;
		colorC[1] = 255;
		colorC[2] = 255;
	}

	//Green Pixel
	if(colorflag == 2)
	{
		colorC[0] = 0;
		colorC[1] = 255;
		colorC[2] = 0;
	}

	//Red Pixel
	if(colorflag == 3)
	{
		colorC[0] = 0;
		colorC[1] = 0;
		colorC[2] = 255;
	}
}

/*
	Function to encode each pixel based on x and y	
*/
string intToString(int x, int y)
{
	string s = "";
	stringstream ss;

	ss << x;
	s += ss.str() + "-";
	ss.str("");
	ss << y;
	s += ss.str();

	return s;
}

grow::grow(int threshold)
{
	this->threshold = threshold;
}

/*
	Region Growing algorithm, which is flood type algorithm
	edgeMap -> output images with only edges of final blobs
	sX --> Seed Pixel x value
	sY --> Seed Pixel y value
	threshold --> if distance is less than threshold then recursion proceeds, else stops.
	colorflag --> to determine the color to be filled with
*/
grow::start_grow(Mat input, Mat edgeMap, int sX, int sY, int colorflag)
{
	int x, y;
	long int count = 1;
	String s = "";
	Vec3b seed = input.at<Vec3b>(sX, sY);
	vector< vector<bool> > reach(input.rows, vector<bool>(input.cols, false));

	list <string> queue;

	reach[sX][sY] = true;

	modifyPixel(input, sX, sY, colorflag);

	s = intToString(sX, sY);
	queue.push_back(s);

	while(!queue.empty())
	{
		s = queue.front();
		queue.pop_front();

		std::size_t pos = s.find('-');
		stringstream ssx(s.substr(0, pos));
		stringstream ssy(s.substr(pos + 1));
		ssx >> x;
		ssy >> y;

		//Right Pixel
		if(x + 1 < input.rows && (!reach[x + 1][y]))
		{
			if(colorDistance(seed, input.at<Vec3b>(x + 1, y), threshold))
			{
				//cout << "reached right pixel" << endl;
				reach[x + 1][y] = true;
				s = intToString(x + 1, y);
				queue.push_back(s);
				//updateMean(seed, input.at<Vec3b>(x + 1, y), count);
				modifyPixel(input, x + 1, y, colorflag);
				++count;
			}
			else
				modifyPixel(edgeMap, x + 1, y, colorflag);

		}

		//Below Pixel
		if(y + 1 < input.cols && (!reach[x][y + 1]))
		{
			if(colorDistance(seed, input.at<Vec3b>(x, y + 1), threshold))
			{
				//cout << "reached Below pixel" << endl;
				reach[x][y + 1] = true;
				s = intToString(x, y + 1);
				queue.push_back(s);
				//updateMean(seed, input.at<Vec3b>(x, y + 1), count);
				modifyPixel(input, x, y + 1, colorflag);
				++count;
			}
			else
				modifyPixel(edgeMap, x, y + 1, colorflag);
		}

		//cout << "+++seed Pixel" << endl;
		//cout << "seed[0] : " << (int)seed[0] << ", seed[1] : " << (int)seed[1] << ", seed[2] = " << (int)seed[2] << endl;

		//Left Pixel
		if(x - 1 >= 0 && (!reach[x - 1][y]))
		{
			if(colorDistance(seed, input.at<Vec3b>(x - 1, y), threshold))
			{
				//cout << "reached left pixel" << endl;
				reach[x - 1][y] = true;
				s = intToString(x - 1, y);
				queue.push_back(s);
				//updateMean(seed, input.at<Vec3b>(x - 1, y), count);
				modifyPixel(input, x - 1, y, colorflag);
				++count;
			}
			else
				modifyPixel(edgeMap, x - 1, y, colorflag);
		}

		//Above Pixel
		if(y - 1 >= 0 && (!reach[x][y - 1]))
		{
			if(colorDistance(seed, input.at<Vec3b>(x, y - 1), threshold))
			{
				//cout << "reached Above pixel" << endl;
				reach[x][y - 1] = true;
				s = intToString(x, y - 1);
				queue.push_back(s);
				//updateMean(seed, input.at<Vec3b>(x, y - 1), count);
				modifyPixel(input, x, y - 1, colorflag);
				++count;
			}
			else
				modifyPixel(edgeMap, x, y - 1, colorflag);
		}

		//Bottom Right Pixel
		if(x + 1 < input.rows && y + 1 < input.cols && (!reach[x + 1][y + 1]))
		{
			if(colorDistance(seed, input.at<Vec3b>(x + 1, y + 1), threshold))
			{
				//cout << "reached Bottom Right pixel" << endl;
				reach[x + 1][y + 1] = true;
				s = intToString(x + 1, y + 1);
				queue.push_back(s);
				//updateMean(seed, input.at<Vec3b>(x + 1, y + 1), count);
				modifyPixel(input, x + 1, y + 1, colorflag);
				++count;
			}
			else
				modifyPixel(edgeMap, x + 1, y + 1, colorflag);
		}

		//Upper Right Pixel
		if(x + 1 < input.rows && y - 1 >= 0 && (!reach[x + 1][y - 1]))
		{
			if(colorDistance(seed, input.at<Vec3b>(x + 1, y - 1), threshold))
			{
				//cout << "reached Upper right pixel" << endl;
				reach[x + 1][y - 1] = true;
				s = intToString(x + 1, y - 1);
				queue.push_back(s);
				//updateMean(seed, input.at<Vec3b>(x + 1, y - 1), count);
				modifyPixel(input, x + 1, y - 1, colorflag);
				++count;
			}
			else
				modifyPixel(edgeMap, x + 1, y - 1, colorflag);
		}

		//Bottom Left Pixel
		if(x - 1 >= 0 && y + 1 < input.cols && (!reach[x - 1][y + 1]))
		{
			if(colorDistance(seed, input.at<Vec3b>(x - 1, y + 1), threshold))
			{
				//cout << "reached Bottom left pixel" << endl;			
				reach[x - 1][y + 1] = true;
				s = intToString(x - 1, y + 1);
				queue.push_back(s);
				//updateMean(seed, input.at<Vec3b>(x - 1, y + 1), count);
				modifyPixel(input, x - 1, y + 1, colorflag);
				++count;
			}
			else
				modifyPixel(edgeMap, x - 1, y + 1, colorflag);
		}

		//Upper left Pixel
		if(x - 1 >= 0 && y - 1 >= 0 && (!reach[x - 1][y - 1]))
		{
			if(colorDistance(seed, input.at<Vec3b>(x - 1, y - 1), threshold))
			{
				//cout << "reached Upper left pixel" << endl;
				reach[x - 1][y - 1] = true;
				s = intToString(x - 1, y - 1);
				queue.push_back(s);
				//updateMean(seed, input.at<Vec3b>(x - 1, y - 1), count);
				modifyPixel(input, x - 1, y - 1, colorflag);
				++count;
			}
			else
				modifyPixel(edgeMap, x - 1, y - 1, colorflag);
		}
	}
}

grow::setThreshold(int threshold)
{
	this->threshold = threshold;
}

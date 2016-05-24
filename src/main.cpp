#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <sstream>
#include <list>
#include "Color.cpp"
#include <chrono>
//#include "chrono_io"

using namespace std;
using namespace cv;
using namespace std::chrono;

Vec3b seed;

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
void modifyPixel(Mat input, Vec3b seed, int x, int y, int colorflag)
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

/*
	Function to update the mean value of region to seed pixel
*/
void updateMean(Vec3b &seed, Vec3b b, long int count)
{
	int s0, s1, s2;
	s0 = (int)((count * seed[0] + b[0]) / (count + 1));
	s1 = (int)((count * seed[1] + b[1]) / (count + 1));
	s2 = (int)((count * seed[2] + b[2]) / (count + 1));

	seed[0] = s0;
	seed[1] = s1;
	seed[2] = s2;
}

/*
	Region Growing algorithm, which is BFS type algorithm
	sX --> Seed Pixel x value
	sY --> Seed Pixel y value
	threshold --> if distance is less than threshold then recursion proceeds, else stops.
	colorflag --> to determine the color to be filled with
*/
void grow(Mat input, int sX, int sY, int threshold, int colorflag)
{
	int x, y;
	long int count = 1;
	String s = "";
	vector< vector<bool> > reach;

	for (int i = 0; i < input.rows; ++i)
	{
		reach.push_back(vector<bool>());
		for (int j = 0; j < input.cols; ++j)
		{
			reach[i].push_back(false);
		}
	}

	list <string> queue;

	reach[sX][sY] = true;

	modifyPixel(input, seed, sX, sY, colorflag);

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
		if(x + 1 < input.rows && (!reach[x + 1][y]) && colorDistance(seed, input.at<Vec3b>(x + 1, y), threshold))
		{
			//cout << "reached right pixel" << endl;
			reach[x + 1][y] = true;
			s = intToString(x + 1, y);
			queue.push_back(s);
			//updateMean(seed, input.at<Vec3b>(x + 1, y), count);
			modifyPixel(input, seed, x + 1, y, colorflag);
			++count;

		}

		//Below Pixel
		if(y + 1 < input.cols && (!reach[x][y + 1]) && colorDistance(seed, input.at<Vec3b>(x, y + 1), threshold))
		{
			//cout << "reached Below pixel" << endl;
			reach[x][y + 1] = true;
			s = intToString(x, y + 1);
			queue.push_back(s);
			//updateMean(seed, input.at<Vec3b>(x, y + 1), count);
			modifyPixel(input, seed, x, y + 1, colorflag);
			++count;
		}

		//cout << "+++seed Pixel" << endl;
		//cout << "seed[0] : " << (int)seed[0] << ", seed[1] : " << (int)seed[1] << ", seed[2] = " << (int)seed[2] << endl;

		//Left Pixel
		if(x - 1 >= 0 && (!reach[x - 1][y]) && colorDistance(seed, input.at<Vec3b>(x - 1, y), threshold))
		{
			//cout << "reached left pixel" << endl;
			reach[x - 1][y] = true;
			s = intToString(x - 1, y);
			queue.push_back(s);
			//updateMean(seed, input.at<Vec3b>(x - 1, y), count);
			modifyPixel(input, seed, x - 1, y, colorflag);
			++count;
		}

		//Above Pixel
		if(y - 1 >= 0 && (!reach[x][y - 1]) && colorDistance(seed, input.at<Vec3b>(x, y - 1), threshold))
		{
			//cout << "reached Above pixel" << endl;
			reach[x][y - 1] = true;
			s = intToString(x, y - 1);
			queue.push_back(s);
			//updateMean(seed, input.at<Vec3b>(x, y - 1), count);
			modifyPixel(input, seed, x, y - 1, colorflag);
			++count;
		}

		//Bottom Right Pixel
		if(x + 1 < input.rows && y + 1 < input.cols && (!reach[x + 1][y + 1]) && colorDistance(seed, input.at<Vec3b>(x + 1, y + 1), threshold))
		{
			//cout << "reached Bottom Right pixel" << endl;
			reach[x + 1][y + 1] = true;
			s = intToString(x + 1, y + 1);
			queue.push_back(s);
			//updateMean(seed, input.at<Vec3b>(x + 1, y + 1), count);
			modifyPixel(input, seed, x + 1, y + 1, colorflag);
			++count;
		}

		//Upper Right Pixel
		if(x + 1 < input.rows && y - 1 >= 0 && (!reach[x + 1][y - 1]) && colorDistance(seed, input.at<Vec3b>(x + 1, y - 1), threshold))
		{
			//cout << "reached Upper right pixel" << endl;
			reach[x + 1][y - 1] = true;
			s = intToString(x + 1, y - 1);
			queue.push_back(s);
			//updateMean(seed, input.at<Vec3b>(x + 1, y - 1), count);
			modifyPixel(input, seed, x + 1, y - 1, colorflag);
			++count;
		}

		//Bottom Left Pixel
		if(x - 1 >= 0 && y + 1 < input.cols && (!reach[x - 1][y + 1]) && colorDistance(seed, input.at<Vec3b>(x - 1, y + 1), threshold))
		{
			//cout << "reached Bottom left pixel" << endl;
			reach[x - 1][y + 1] = true;
			s = intToString(x - 1, y + 1);
			queue.push_back(s);
			//updateMean(seed, input.at<Vec3b>(x - 1, y + 1), count);
			modifyPixel(input, seed, x - 1, y + 1, colorflag);
			++count;
		}

		//Upper left Pixel
		if(x - 1 >= 0 && y - 1 >= 0 && (!reach[x - 1][y - 1]) && colorDistance(seed, input.at<Vec3b>(x - 1, y - 1), threshold))
		{
			//cout << "reached Upper left pixel" << endl;
			reach[x - 1][y - 1] = true;
			s = intToString(x - 1, y - 1);
			queue.push_back(s);
			//updateMean(seed, input.at<Vec3b>(x - 1, y - 1), count);
			modifyPixel(input, seed, x - 1, y - 1, colorflag);
			++count;
		}
	}	

}



int main(int argc, char const **argv)
{
	Mat src, org, deniose;

	src = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	fastNlMeansDenoisingColored(src, deniose, 2);
	medianBlur(deniose, deniose, 3);

	deniose.copyTo(org);

	typedef std::chrono::high_resolution_clock clk;

	auto t1 = clk::now();

	seed = deniose.at<Vec3b>(290, 290);
	grow(deniose, 290, 290, 20, 1);

	auto t2 = clk::now();

	seed = deniose.at<Vec3b>(256, 294);
	grow(deniose, 256, 294, 20, 1);

	auto t3 = clk::now();

	seed = deniose.at<Vec3b>(303, 515);
	grow(deniose, 303, 515, 20, 3);

	auto t4 = clk::now();

	cout << "First seed : " << duration_cast<microseconds>(t2 - t1).count() << ", Second seed : " << duration_cast<microseconds>(t3 - t2).count() << ", Third seed : " << duration_cast<microseconds>(t4 - t3).count() << endl;

	imshow("Original Image", org);
	imshow("Modified Image", deniose);
	waitKey(0);
	destroyAllWindows();

	return 0;
}
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <sstream>
#include <list>

using namespace std;
using namespace cv;

Vec3b seed;

/*
	Function to determine the color distance between two pixel points
	thOrdinary --> threshold for Normal Pixels
	thwhite    --> threshold for Overexposed Pixels	
*/
bool colorDistance(Vec3b a,Vec3b b, int thOrdinary, int thWhite)
{
    float a1 = a[2];
    float a2 = a[1];
    float a3 = a[0];

    float b1 = b[2];
    float b2 = b[1];
    float b3 = b[0];

    float dist1 = (((a1 - b1) * (a1 - b1) + (a2 - b2) * (a2 - b2) + (a3 - b3) * (a3 - b3)));
    dist1 = sqrt(dist1 / 3);

    float dist2 = (((255 - b1) * (255 - b1) + (255 - b2) * (255 - b2) + (255 - b3) * (255 - b3)));
    dist2 = sqrt(dist2 / 3);
    
    if(dist1 <= thOrdinary || dist2 <= thWhite)
    	return true;
    else
    	return false;
}

/*
	Function to modify pixel values at a point as per seed pixel
*/
void modifyPixel(Mat input, Vec3b seed, int x, int y)
{
	Vec3b &colorC = input.at<Vec3b>(x, y);
	colorC[0] = 0;
	colorC[1] = 255;
	colorC[2] = 255;
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
	thOrdinary --> threshold for normal Pixels
	thWhite    --> threshold for Overexposed Pixels
*/
void grow(Mat input, int sX, int sY, int thOrdinary, int thWhite)
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

	modifyPixel(input, seed, sX, sY);

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
		if(x + 1 < input.rows && (!reach[x + 1][y]) && colorDistance(seed, input.at<Vec3b>(x + 1, y), thOrdinary, thWhite))
		{
			cout << "reached right pixel" << endl;
			reach[x + 1][y] = true;
			s = intToString(x + 1, y);
			queue.push_back(s);
			updateMean(seed, input.at<Vec3b>(x + 1, y), count);
			modifyPixel(input, seed, x + 1, y);
			++count;

		}

		//Below Pixel
		if(y + 1 < input.cols && (!reach[x][y + 1]) && colorDistance(seed, input.at<Vec3b>(x, y + 1), thOrdinary, thWhite))
		{
			cout << "reached Below pixel" << endl;
			reach[x][y + 1] = true;
			s = intToString(x, y + 1);
			queue.push_back(s);
			updateMean(seed, input.at<Vec3b>(x, y + 1), count);
			modifyPixel(input, seed, x, y + 1);
			++count;
		}

		cout << "+++seed Pixel" << endl;
		cout << "seed[0] : " << (int)seed[0] << ", seed[1] : " << (int)seed[1] << ", seed[2] = " << (int)seed[2] << endl;

		//Left Pixel
		if(x - 1 >= 0 && (!reach[x - 1][y]) && colorDistance(seed, input.at<Vec3b>(x - 1, y), thOrdinary, thWhite))
		{
			cout << "reached left pixel" << endl;
			reach[x - 1][y] = true;
			s = intToString(x - 1, y);
			queue.push_back(s);
			updateMean(seed, input.at<Vec3b>(x - 1, y), count);
			modifyPixel(input, seed, x - 1, y);
			++count;
		}

		//Above Pixel
		if(y - 1 >= 0 && (!reach[x][y - 1]) && colorDistance(seed, input.at<Vec3b>(x, y - 1), thOrdinary, thWhite))
		{
			cout << "reached Above pixel" << endl;
			reach[x][y - 1] = true;
			s = intToString(x, y - 1);
			queue.push_back(s);
			updateMean(seed, input.at<Vec3b>(x, y - 1), count);
			modifyPixel(input, seed, x, y - 1);
			++count;
		}

		//Bottom Right Pixel
		if(x + 1 < input.rows && y + 1 < input.cols && (!reach[x + 1][y + 1]) && colorDistance(seed, input.at<Vec3b>(x + 1, y + 1), thOrdinary, thWhite))
		{
			cout << "reached Bottom Right pixel" << endl;
			reach[x + 1][y + 1] = true;
			s = intToString(x + 1, y + 1);
			queue.push_back(s);
			updateMean(seed, input.at<Vec3b>(x + 1, y + 1), count);
			modifyPixel(input, seed, x + 1, y + 1);
			++count;
		}

		//Upper Right Pixel
		if(x + 1 < input.rows && y - 1 >= 0 && (!reach[x + 1][y - 1]) && colorDistance(seed, input.at<Vec3b>(x + 1, y - 1), thOrdinary, thWhite))
		{
			cout << "reached Upper right pixel" << endl;
			reach[x + 1][y - 1] = true;
			s = intToString(x + 1, y - 1);
			queue.push_back(s);
			updateMean(seed, input.at<Vec3b>(x + 1, y - 1), count);
			modifyPixel(input, seed, x + 1, y - 1);
			++count;
		}

		//Bottom Left Pixel
		if(x - 1 >= 0 && y + 1 < input.cols && (!reach[x - 1][y + 1]) && colorDistance(seed, input.at<Vec3b>(x - 1, y + 1), thOrdinary, thWhite))
		{
			cout << "reached Bottom left pixel" << endl;
			reach[x - 1][y + 1] = true;
			s = intToString(x - 1, y + 1);
			queue.push_back(s);
			updateMean(seed, input.at<Vec3b>(x - 1, y + 1), count);
			modifyPixel(input, seed, x - 1, y + 1);
			++count;
		}

		//Upper left Pixel
		if(x - 1 >= 0 && y - 1 >= 0 && (!reach[x - 1][y - 1]) && colorDistance(seed, input.at<Vec3b>(x - 1, y - 1), thOrdinary, thWhite))
		{
			cout << "reached Upper left pixel" << endl;
			reach[x - 1][y - 1] = true;
			s = intToString(x - 1, y - 1);
			queue.push_back(s);
			updateMean(seed, input.at<Vec3b>(x - 1, y - 1), count);
			modifyPixel(input, seed, x - 1, y - 1);
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

	seed = deniose.at<Vec3b>(280, 290);

	grow(deniose, 280, 290, 50, 200);
	
	imshow("Original Image", org);
	imshow("Modified Image", deniose);
	waitKey(0);
	destroyAllWindows();

	return 0;
}
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

Vec3b seed;


bool colorDistance(Vec3b a,Vec3b b, int threshold)
{
    float a1=a[2];
    float a2=a[1];
    float a3=a[0];

    float b1=b[2];
    float b2=b[1];
    float b3=b[0];

    float dist=(((a1-b1)*(a1-b1)+(a2-b2)*(a2-b2)+(a3-b3)*(a3-b3)));
    dist=sqrt(dist/3);
    
    if(dist <= threshold)
    	return true;
    else
    	return false;
}


//void regionGrowing(Mat input, int x, int y, int threshold, vector< vector<bool> > &reach)
//{
	
//}

void modifyPixel(Mat &input, Vec3b seed, int x, int y)
{
	Vec3b &colorC = input.at<Vec3b>(x, y);
	colorC[0] = seed[0];
	colorC[1] = seed[1];
	colorC[2] = seed[2];
}

void grow(Mat &input, int x, int y, vector< vector<bool> > &reach, int threshold)
{
	if(x >= 0 && x < input.rows && y >= 0 && y < input.cols && (!reach[x][y]) && colorDistance(seed, input.at<Vec3b>(x, y), threshold))
	{
		reach[x][y] = true;
		modifyPixel(input, seed, x, y);
		
		//Update seed pixel


		grow(input, x + 1, y, reach, threshold);
		grow(input, x, y + 1, reach, threshold);
		grow(input, x - 1, y, reach, threshold);
		grow(input, x, y - 1, reach, threshold);

	}
	else
		return;
}

int main(int argc, char const **argv)
{
	vector< vector<bool> > reach;
	Mat src, org, deniose;
	src = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	cout << "reach" <<endl;
	//src.copyTo(deniose);
	fastNlMeansDenoisingColored(src, deniose, 2);
	medianBlur(deniose, deniose, 3);
	deniose.copyTo(org);

	for (int i = 0; i < deniose.rows; ++i)
	{
		reach.push_back(vector<bool>());
		for (int j = 0; j < deniose.cols; ++j)
		{
			reach[i].push_back(false);
		}
	}

	seed = deniose.at<Vec3b>(290, 280);

	grow(deniose, 290, 280, reach, 50);
	//cout << "reached" << endl;
	imshow("Original Image", org);
	imshow("Modified Image", deniose);
	waitKey(0);
	destroyAllWindows();

	return 0;
}
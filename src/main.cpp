#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <sstream>
#include <list>
#include "Color.cpp"
#include <chrono>
#include "grow.cpp"
//#include "chrono_io"

using namespace std;
using namespace cv;
using namespace std::chrono;

int main(int argc, char const **argv)
{
	Mat src, org, denoise;

	src = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	fastNlMeansDenoisingColored(src, denoise, 2);
	medianBlur(denoise, denoise, 3);

	denoise.copyTo(org);
	Mat edgeMap(denoise.rows, denoise.cols, CV_8UC3, Scalar(0, 0, 0));

	typedef std::chrono::high_resolution_clock clk;

	auto t1 = clk::now();

	seed = denoise.at<Vec3b>(290, 290);
	grow(denoise, edgeMap, 290, 290, 20, 1);

	auto t2 = clk::now();

	seed = denoise.at<Vec3b>(256, 294);
	grow(denoise, edgeMap,256, 294, 20, 1);

	auto t3 = clk::now();

	seed = denoise.at<Vec3b>(303, 515);

	grow(denoise, edgeMap, 303, 515, 20, 3);

	auto t4 = clk::now();

	cout << "First seed : " << duration_cast<microseconds>(t2 - t1).count() << ", Second seed : " << duration_cast<microseconds>(t3 - t2).count() << ", Third seed : " << duration_cast<microseconds>(t4 - t3).count() << endl;

	imshow("Original Image", org);
	imshow("Modified Image", denoise);
	imshow("EdgeMap Image", edgeMap);
	waitKey(0);
	destroyAllWindows();

	return 0;
}
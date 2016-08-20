#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <sstream>
#include <list>
#include <bits/stdc++.h>
#include "Color.hpp"
#include "grow.h"

using namespace std;
using namespace cv;

/*
        Function to determine the color difference between two pixel points.
        This function also calculates the color distance between white pixel
        and input pixel to deal overexposed regions of buoy.
        This function implements CIEDE2000 algorithm to find the distance in color
 */
bool colorDistance(const Vec3b a, const Vec3b b, const int colorThreshold, const int whiteThreshold)
{
    std::vector<double> ca, cb, cw(3, 255);

    ca.push_back((double) a[2]);
    ca.push_back((double) a[1]);
    ca.push_back((double) a[0]);

    cb.push_back((double) b[2]);
    cb.push_back((double) b[1]);
    cb.push_back((double) b[0]);

    float dist1 = kallaballa::ciede2000_distance(ca, cb);
    float dist2 = kallaballa::ciede2000_distance(cb, cw);

    if ((int) dist1 <= colorThreshold || (int) dist2 <= whiteThreshold)
        return true;
    else
        return false;
}

/*
        Function to modify pixel values at a point as per seed pixel
 */
void modifyPixel(Mat input, const int x, const int y, const int colorflag)
{
    Vec3b &colorC = input.at<Vec3b>(x, y);

    //Yellow Pixel
    if (colorflag == 2)
    {
        colorC[0] = 0;
        colorC[1] = 255;
        colorC[2] = 255;
    }

    //Green Pixel
    if (colorflag == 3)
    {
        colorC[0] = 0;
        colorC[1] = 255;
        colorC[2] = 0;
    }

    //Red Pixel
    if (colorflag == 4)
    {
        colorC[0] = 0;
        colorC[1] = 0;
        colorC[2] = 255;
    }
}

/*
        Constructor sets default values to colorThreshold and whiteThreshold
 */
grow::grow(int colorThreshold, int whiteThreshold) : colorThreshold(colorThreshold), whiteThreshold(whiteThreshold)
{

}

/*
        Region Growing algorithm, which is flood type algorithm
        filled -> output image with filled buoys
        edgeMap -> output image with only edges of final blobs
        sX --> Seed Pixel x value
        sY --> Seed Pixel y value
        colorflag --> to determine the color to be filled with
 */
void grow::start_grow(Mat input, Mat filled, Mat edgeMap, const int sX, const int sY, const int colorflag)
{
    int x, y;
    long int count = 1;
    String s = "";
    Vec3b seed = input.at<Vec3b>(sX, sY);
    vector < vector<bool> > reach(input.rows, vector<bool>(input.cols, false));

    list < pair<int, int> > queue;

    reach[sX][sY] = true;

    modifyPixel(filled, sX, sY, colorflag);

    queue.push_back(make_pair(sX, sY));

    auto helper_lambda = [&](int lx, int ly)
        {
            if (colorDistance(seed, input.at<Vec3b>(lx , ly), colorThreshold, whiteThreshold))
            {
                reach[lx][ly] = true;
                queue.push_back(make_pair(lx, ly));
                modifyPixel(filled, lx, ly, colorflag);
                ++count;
            }
            else
                modifyPixel(edgeMap, lx, ly, colorflag);
        };

    while (!queue.empty())
    {
        x = queue.front().first;
        y = queue.front().second;
        queue.pop_front();

        //Right Pixel
        if (x + 1 < input.rows && (!reach[x + 1][y]))
            helper_lambda(x + 1, y);

        //Below Pixel
        if (y + 1 < input.cols && (!reach[x][y + 1]))
            helper_lambda(x, y + 1);

        //Left Pixel
        if (x - 1 >= 0 && (!reach[x - 1][y]))
            helper_lambda(x - 1, y);

        //Above Pixel
        if (y - 1 >= 0 && (!reach[x][y - 1]))
            helper_lambda(x, y - 1);

        //Bottom Right Pixel
        if (x + 1 < input.rows && y + 1 < input.cols && (!reach[x + 1][y + 1]))
            helper_lambda(x + 1, y + 1);

        //Upper Right Pixel
        if (x + 1 < input.rows && y - 1 >= 0 && (!reach[x + 1][y - 1]))
            helper_lambda(x + 1, y - 1);

        //Bottom Left Pixel
        if (x - 1 >= 0 && y + 1 < input.cols && (!reach[x - 1][y + 1]))
            helper_lambda(x - 1, y + 1);

        //Upper left Pixel
        if (x - 1 >= 0 && y - 1 >= 0 && (!reach[x - 1][y - 1]))
            helper_lambda(x - 1, y - 1);
    }
}

/*
        Funtion to set both the thresholds  
 */
void grow::setThresholds(const int colorThreshold, const int whiteThreshold)
{
    this->colorThreshold = colorThreshold;
    this->whiteThreshold = whiteThreshold;
}
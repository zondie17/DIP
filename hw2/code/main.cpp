/**
 * Image Warping
 * 2020/03/14
 * zyw
 * */
#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <cmath>
#include <Eigen/Eigen>
#include <Eigen/QR>
using namespace cv;
using std::cout, std::endl;
using std::vector;
//#define DEBUG

Vec3b interpolate(const Mat& src, double x, int xf, int xc, double y, int yf, int yc) {
    Vec3f res;
    if (xf == xc && yf == yc)
        res = src.at<Vec3b>(xf, yf);
    else if (xf == xc)
        res = src.at<Vec3b>(xf, yf) * (yc-y) + src.at<Vec3b>(xf, yc) * (y-yf);
    else if (yf == yc)
        res = src.at<Vec3b>(xf, yf) * (xc-x) + src.at<Vec3b>(xc, yf) * (x-xf);
    else {
        Vec3f inter1 = src.at<Vec3b>(xf, yf) * (yc-y) + src.at<Vec3b>(xf, yc) * (y-yf);
        Vec3f inter2 = src.at<Vec3b>(xc, yf) * (yc-y) + src.at<Vec3b>(xc, yc) * (y-yf);
        res = inter1 * (xc-x) + inter2 * (x-xf);
    }
    Vec3b res2 = res;
    return res2;
}

void affine(const Mat& source, const Mat& target, Mat& result, const int Sx[], const int Sy[], const int Tx[], const int Ty[]) {
    Eigen::Matrix3f srcA;
    srcA << Sx[0], Sy[0], 1, Sx[1], Sy[1], 1, Sx[2], Sy[2], 1;
    Eigen::Vector3f b1;
    b1 << Tx[0], Tx[1], Tx[2];
    Eigen::Vector3f x1 = srcA.colPivHouseholderQr().solve(b1);
    Eigen::Vector3f b2;
    b2 << Ty[0], Ty[1], Ty[2];
    Eigen::Vector3f x2 = srcA.colPivHouseholderQr().solve(b2);
    Eigen::Matrix3f A;
    A << x1(0), x1(1), x1(2), x2(0), x2(1), x2(2), 0, 0, 1;
    int width = result.cols;
    int height = result.rows;
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j) {
            Eigen::Vector3f vecB;
            vecB << i, j, 1;
            Eigen::Vector3f vecX = A.colPivHouseholderQr().solve(vecB);

            double x = vecX(0);
            double y = vecX(1);
            if (0 <= x && x < source.rows && 0 <= y && y < source.cols)
                result.at<Vec3b>(i,j) = interpolate(source, x, floor(x), ceil(x), y, floor(y), ceil(y));
            else
                result.at<Vec3b>(i,j) = target.at<Vec3b>(i,j);

#ifdef DEBUG
            int x = vecX(0);
            int y = vecX(1);
            float X = vecX(0);
            float Y = vecX(1);
            if (0 <= x && x < source.rows && 0 <= y && y < source.cols) {
                result.at<Vec3b>(i,j) = (source.at<Vec3b>(x,y) * ((float)x+1-X)*((float)y+1-Y)
                                         + source.at<Vec3b>(x,y+1) * ((float)x+1-X)*(Y-(float)y)
                                         + source.at<Vec3b>(x+1,y) * (X-(float)x)*((float)y+1-Y)
                                         + source.at<Vec3b>(x+1,y+1) * (X-(float)x)*(Y-(float)y));
                // result.at<Vec3b>(i,j) = source.at<Vec3b>(round(vecX(0)), round(vecX(1)));
            }
            else {
                result.at<Vec3b>(i,j) = target.at<Vec3b>(i,j);
            }
#endif
        }
}

void projective(const Mat &in, Mat& out) {
    double rhoMax = (double)min(out.rows, out.cols) / 2;
    double dMax = (double)max(in.rows, in.cols) / 2;
    for (int i = -out.rows/2; i < out.rows/2; ++i)
        for (int j = -out.cols/2; j < out.cols/2; ++j) {
            double rho = sqrt(i*i+j*j);
            double theta = atan2(i,j);
            if (rho/rhoMax > 1)
                continue;
            double phi = asin(rho/rhoMax);

            double d = 2.0 / M_PI * dMax * phi;
            double x = d * sin(theta);
            double y = d * cos(theta);

            if (-in.rows/2.0 <= x && x <= in.rows/2.0 && -in.cols/2.0 <= y && y <= in.cols/2.0)
                out.at<Vec3b>(i+out.rows/2, j+out.cols/2) = in.at<Vec3b>((int)round(x+in.rows/2.0), (int)round(y+in.cols/2.0));
            else
                out.at<Vec3b>(i+out.rows/2, j+out.cols/2) = Vec3b(127, 127, 127);
        }
}

void cart2pol(const Mat &in, Mat& out, int size) {
    int width = in.cols;
    int height = in.rows;

    double R = (size - 1) / 2.0;
    double deltaR = (2.0 * height) / (size - 1);
    double deltaT = 2.0 * M_PI / width;

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) {
            double x = j - R;
            double y = R - i;
            double r = sqrt(x*x + y*y);
            if (r > R)
                continue;

            double theta = atan2(y, x);
            theta = theta > 0 ? theta : (theta + 2.0*M_PI);
            double cx = r * deltaR;
            double cy = theta / deltaT;
            int xf = floor(cx) > 0 ? (int)floor(cx) : 0;
            int xc = ceil(cx) < height ? (int)ceil(cx) : (height-1);
            int yf = floor(cy) > 0 ? (int)floor(cy) : 0;
            int yc = ceil(cy) < width ? (int)ceil(cy) : (width-1);

            out.at<Vec3b>(i, j) = interpolate(in, cx, xf, xc, cy, yf, yc);
        }
}

int main() {
    // 1
    Mat source = imread("../image/source.jpg");
    Mat target = imread("../image/target.jpg");
    Mat result(source.size(), CV_8UC3);
    int Sx[4] = {0, 524, 0, 524};
    int Sy[4] = {0, 0, 699, 699};
    int Tx[4] = {193, 315, 265, 387};
    int Ty[4] = {192, 168, 535, 511};
    affine(source, target, result, Sx, Sy, Tx, Ty);
    imwrite("../image/result.jpg", result);

    // 2
    int rOut = 300, cOut = 300;
    Mat warping = imread("../image/warping.png");
    Mat warping_result(warping.size(), CV_8UC3);
    projective(warping, warping_result);
    imwrite("../image/warping_result.png", warping_result);

    // 3
    Mat input = imread("../image/cart4.jpg");
    Mat output = Mat::zeros(500, 500, CV_8UC3);
    cart2pol(input, output, output.rows);
    imwrite("../image/polar4.jpg", output);
    return 0;
}


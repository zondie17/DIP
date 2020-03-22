/**
 * Point Processing
 * 2020/02/29
 * zyw
 * */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <cmath>
using namespace cv;
using std::cout, std::endl;
using std::vector;
// #define DEBUG

// 亮度改变
void dimmer(const Mat& input, Mat& output, int g) {
    int width = input.cols;
    int height = input.rows;
    for (int bgr = 0; bgr < 3; ++bgr)
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j) {
                int num = input.at<Vec3b>(i, j)[bgr];
                num = max(min(num + g, 255), 0);
                output.at<Vec3b>(i, j)[bgr] = num;
            }
}

// 对比度放缩
void contrast(const Mat& input, Mat& output, const int maxJ[], const int minJ[]) {
    int width = input.cols;
    int height = input.rows;
    for (int bgr = 0; bgr < 3; ++bgr) {
        int minI = 255, maxI = 0;
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j) {
                int num = input.at<Vec3b>(i, j)[bgr];
                minI = minI < num ? minI : num;
                maxI = maxI > num ? maxI : num;
            }

        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j) {
                float num = input.at<Vec3b>(i, j)[bgr];
                num = (float)(maxJ[bgr] - minJ[bgr]) * (num - (float)minI) / (float)(maxI - minI) + (float)minJ[bgr];
                num = max(min((int)num, 255), 0);
                output.at<Vec3b>(i, j)[bgr] = (int)num;
            }
    }

}

// 伽马校正
void gamma(const Mat& input, Mat& output, float g) {
    int width = input.cols;
    int height = input.rows;
    for (int bgr = 0; bgr < 3; ++bgr)
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j) {
                float num = input.at<Vec3b>(i, j)[bgr];
                num = 255 * pow(num/255, g);
                num = max(min((int)num, 255), 0);
                output.at<Vec3b>(i, j)[bgr] = num;
            }
}

// 直方图均衡
void equalization(const Mat& input, Mat& output) {
    Mat channels[3];
    split(input, channels);
    for (auto & channel : channels){
        int histSize = 256;
        float range[] = { 0,255 };
        const float*histRanges = { range };
        Mat hist;
        calcHist(&channel, 1, nullptr, Mat(), hist, 1, &histSize, &histRanges, true, false);
        hist = hist / (channel.rows * channel.cols);
        float cdf[256] = {0};
        Mat lut(1, 256, CV_8U);
        cdf[0] = hist.at<float>(0);
        lut.at<uchar>(0) = static_cast<uchar>(255 * cdf[0]);
        for (int i = 1; i < 256; ++i) {
            cdf[i] = cdf[i-1] + hist.at<float>(i);
            lut.at<uchar>(i) = static_cast<uchar>(255 * cdf[i]);
        }
        LUT(channel, lut, channel);
    }
    merge(channels,3, output);
/*
    // 1
    vector<Mat>channels;
    split(input, channels);
    Mat blue, green, red;
    blue = channels.at(0);
    green = channels.at(1);
    red = channels.at(2);
    equalizeHist(blue, blue);
    equalizeHist(green, green);
    equalizeHist(red, red);
    merge(channels, output);
    // 2
    for (auto &channel : channels)
        equalizeHist(channel, channel);
    */
}

// 匹配
void matching(const Mat& input, const Mat& target, Mat& output) {
    Mat channels[3];
    Mat channelsTarget[3];
    split(input, channels);
    split(target, channelsTarget);

    for (int rgb = 0; rgb < 3; ++rgb) {
        int histSize = 256;
        float range[] = { 0,255 };
        const float*histRanges = { range };
        Mat inputHist, targetHist;
        calcHist(&channels[rgb], 1, nullptr, Mat(), inputHist, 1, &histSize, &histRanges, true, false);
        calcHist(&channelsTarget[rgb], 1, nullptr, Mat(), targetHist, 1, &histSize, &histRanges, true, false);
        inputHist = inputHist / (channels[rgb].rows * channels[rgb].cols);
        targetHist = targetHist / (channelsTarget[rgb].rows * channelsTarget[rgb].cols);
        float inputCDF[256] = {0};
        float targetCDF[256] = {0};

        inputCDF[0] = inputHist.at<float>(0);
        targetCDF[0] = targetHist.at<float>(0);
        for (int i = 0; i < 256; ++i) {
            inputCDF[i] = inputCDF[i-1] + inputHist.at<float>(i);
            targetCDF[i] = targetCDF[i-1] + targetHist.at<float>(i);
        }

        float diff[256][256];
        for (int i = 0; i < 256; ++i)
            for (int j = 0; j < 256; ++j)
                diff[i][j] = fabs(inputCDF[i] - targetCDF[j]);

        Mat lut(1, 256, CV_8U);
        for (int i = 0; i < 256; ++i) {
            float min = diff[i][0];
            int idx = 0;
            for (int j = 1; j < 256; ++j)
                if (min > diff[i][j]) {
                    min = diff[i][j];
                    idx = j;
                }
            lut.at<uchar>(i) = static_cast<uchar>(idx);
        }

        LUT(channels[rgb], lut, channels[rgb]);
    }
    merge(channels, 3, output);
}


int main() {
    Mat img = imread("../image/car.jpg");
    Mat target = imread("../image/target.jpg");

    // 1
    Mat dimmerImg1(img.size(), CV_8UC3);
    dimmer(img, dimmerImg1, 50);
    imwrite("../image/car_dimmer+50.jpg", dimmerImg1);
    Mat dimmerImg2(img.size(), CV_8UC3);
    dimmer(img, dimmerImg2, -50);
    imwrite("../image/car_dimmer-50.jpg", dimmerImg2);

    // 2
    Mat contrastImg1(img.size(), CV_8UC3);
    int maxJ1[]{250,200,150};
    int minJ1[]{0,50,100};
    contrast(img, contrastImg1, maxJ1, minJ1);
    imwrite("../image/car_contrast_bgr.jpg", contrastImg1);
    Mat contrastImg2(img.size(), CV_8UC3);
    int maxJ2[]{150,200,250};
    int minJ2[]{100,50,0};
    contrast(img, contrastImg2, maxJ2, minJ2);
    imwrite("../image/car_contrast_rgb.jpg", contrastImg2);

    // 3
    Mat gammaImg1(img.size(), CV_8UC3);
    gamma(img, gammaImg1, 0.5);
    imwrite("../image/car_gamma_0.5.jpg", gammaImg1);
    Mat gammaImg2(img.size(), CV_8UC3);
    gamma(img, gammaImg2, 2.0);
    imwrite("../image/car_gamma_2.0.jpg", gammaImg2);

    // 4
    Mat equalizeImg(img.size(), CV_8UC3);
    equalization(img, equalizeImg);
    imwrite("../image/car_equalize.jpg", equalizeImg);

    // 5
    Mat matchImg(img.size(), CV_8UC3);
    matching(img, target, matchImg);
    imwrite("../image/matchImg_color.jpg", matchImg);

    return 0;
}
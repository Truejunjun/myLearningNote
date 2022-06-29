#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;
//描述:得到圆心的坐标；
//输入image：输入图片；
//输入f_xy：放缩系数；
//输入min_area：圆的最小面积；
//输入max_area：圆的最大面积；
//输出pt：圆心坐标；
//返回值：成功为0；
int GetCenterCirPoint(Mat image, float f_xy, int min_area, int max_area, Point& pt);

//描述：输入图片的上部分和图片的下部分，最终拼接成完成的图片；
//输入up_image：图片上部分；
//输入down_image：图片下部分；
//输出output：拼接后的图片；
//返回值：成功返回0；
int GetStitchedImage(Mat up_image, Mat down_image, Mat& output);

//描述：从模板图片中得到LED芯片的的位置信息；
//输入image：输入模板图片；
//输出output：得到芯片的有序分布图，从左到右，从上到下；
//返回值：成功返回0；
int GetOrderedDistribution(Mat image, vector<deque<Point>>& output);

//描述：根据模板得到的位置信息，从待检测图片中找到未点亮LED的位置信息；
//输入image：输入待检测图片；
//输入template_：由模板得到的芯片分布位置信息；
//输出x：未点亮LED的横坐标的集合；
//输出y：未点亮LED的纵坐标的集合；
//返回值：返回未点亮LED的数量；
int GetLocationsOfDamagedLEDs(Mat image, vector<deque<Point>>& template_, int* x, int* y);

#include "opencv2/opencv.hpp"
#include <iostream>
#include "ImageRegistration.h"
#include <math.h>
#include <algorithm>
#include <string>

using namespace cv;
using namespace std;
//int GetSubImage(Mat image)
//{
//	resize(image, image, Size(), 0.2, 0.2, INTER_AREA);
//	//blur(image, image, Size(9, 9));
//	//medianBlur(smaller_img, smaller_img, 5);
//	if (image.channels() == 3)
//	{
//		cvtColor(image, image, COLOR_BGR2GRAY);
//	}
//	threshold(image, image, 0, 255, THRESH_OTSU);
//	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
//	Mat morph_img;
//	vector<vector<Point>> contours;
//	vector<Vec4i> hierarchy;
//	morphologyEx(image, image, MORPH_CLOSE, element);
//	findContours(image, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);
//	int x1, x2, y1, y2;
//	Point tl;
//	Point br;
//	int area_value;
//	string area_text;
//	const char* number;
//	double para;
//	for (int i = 0; i < contours.size(); i++)
//	{
//		cout << contourArea(contours[i]) << "  ";
//	//	area_value = contourArea(contours[i]);
//	//	if ((area_value >= 3500) & (area_value < 4500))//正常的检测区域
//	//	{
//	//		area_text = to_string(area_value);
//	//		number = area_text.c_str();
//	//		//cout << area_text << "  ";
//	//		tl = boundingRect(contours[i]).tl() * 10;
//	//		tl = Point(tl.x - 50, tl.y - 50);
//	//		br = boundingRect(contours[i]).br() * 10;
//	//		br = Point(br.x + 50, br.y + 50);
//	//		para = double((double(br.y) - double(tl.y)) / (double(br.x) - double(tl.x)));
//	//		//cout << "左上角：" << tl << " " << "右下角：" << br << endl;
//	//		if ((tl.x != 0) && (tl.y != 0) && (br.x <= img.cols - 10) && (br.y <= img.rows - 10))
//	//		{
//	//			if (para > 1.3 && para < 1.6)//正常的子图
//	//			{
//	//				rectangle(img, tl, br, Scalar(0, 255, 0), 6, 8);
//	//				putText(img, number, tl, FONT_HERSHEY_SIMPLEX, 4, Scalar(0, 255, 0), 8, 8);
//	//				ok_rect.push_back(Rect(tl, br));
//	//			}
//	//			else//有异物的子图
//	//			{
//	//				//rectangle(img, tl, br, Scalar(0, 0, 255), 6, 8);
//	//				//putText(img, number, tl, FONT_HERSHEY_SIMPLEX, 4, Scalar(0, 0, 255), 8, 8);
//	//				//ok_rect.push_back(Rect(tl, br));
//	//			}
//	//		}
//	//	}
//	}
//
//}

//String image_path = "D://002 Repositories//opencvCode//Project1//Project1//1//";
String image_path = "D://myVisualProject//dataset//1";

int main()
{
	//String up_image_path = image_path + "MiniLED_0016.bmp";
	//String down_image_path = image_path + "MiniLED_0015.bmp";
	////读取图片
	//Mat up_image = imread(up_image_path, 1);
	//Mat down_image = imread(down_image_path, 1);
	//Mat dst;
	//double time0 = static_cast<double>(getTickCount());
	//GetStitchedImage(up_image, down_image, dst);
	//cout << "整个程序运行时间：" << ((double)getTickCount() - time0) / getTickFrequency() << endl;
	//cout << "dst的尺寸：" << dst.cols << " " << dst.rows << endl;
	//namedWindow("dst", 0);
	//imshow("dst", dst);

	Mat template_image = imread("D://myVisualProject//dataset//1//MiniLED_0030.bmp", 1);
	Mat image = imread("D://myVisualProject//dataset//1//0030.jpg", 1);
	Size dsize = Size(9344, 7000);
	resize(image, image, dsize, 0, 0, INTER_AREA);

	vector<deque<Point>> rows;
	double time0 = static_cast<double>(getTickCount());
	GetOrderedDistribution(template_image, rows);

	//circle(template_image, rows[34][15] * 4, 20, Scalar(0, 255, 0), 50, 8, 0);
	int x[1000] = {0};
	int y[1000] = {0};
	
	int length = GetLocationsOfDamagedLEDs(image, rows,x,y );
	cout << "整个程序运行时间：" << ((double)getTickCount() - time0) / getTickFrequency() << endl;
	//for (size_t i = 0; i<length; i++)
	//{
	//	cout << x[i] << " " << y[i] << endl;
	//}

	waitKey(0); 
}

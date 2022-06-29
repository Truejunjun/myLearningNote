#include "ImageRegistration.h"

int GetCenterCirPoint(Mat image, float f_xy,int min_area,int max_area, Point& pt)
{
	//Mat smaller;
	resize(image, image, Size(), f_xy, f_xy,INTER_AREA);
	//Mat smaller_gray;
	if (image.channels() == 3) { cvtColor(image, image, COLOR_BGR2GRAY); }//转化边缘检测后的图为灰度图
	medianBlur(image, image, 5);
	threshold(image, image, 0, 255, THRESH_OTSU);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(image, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);
	int area_value = 0;
	Point2f center;//包围点集的最小圆形vector
	float radius = 0;//包围点集的最小圆形半径vector
	//vector<vector<Point>> ExistContours;//筛选目标后轮廓；
	for (int i = 0; i < contours.size(); i++)
	{
		//cout << contourArea(contours[i]) << "  ";
		area_value = contourArea(contours[i]);
		if ((area_value >= min_area) && (area_value < max_area))//正常的检测区域
		{
			minEnclosingCircle(Mat(contours[i]), center, radius);//对于给点集集合，寻找最小面积的包围圆形； //计算并返回包围轮廓点集的最小圆形及其半径
			pt = Point(center.x * (1.0/ f_xy), center.y * (1.0 / f_xy));												//circle(img, Point(center[i].x+ 355,center[i].y+1366), radius[i], Scalar(0, 0, 255), 2, 8, 0);//画出寻找的圆形；
			//circle(image, Point(center.x* (1.0 / f_xy), center.y* (1.0 / f_xy)), radius* (1.0 / f_xy), Scalar(0, 0, 255), 10, 8, 0);

		}
	}
	//namedWindow("image", 0);
	//imshow("image", image);
	return 0;
}

int GetStitchedImage(Mat up_image, Mat down_image, Mat& output)
{
	if (up_image.empty()|| down_image.empty())
	{
		throw "read sub_image failed!";
	}
	//copyMakeBorder(up_image, up_image, 0, 0, 3, 3, BORDER_CONSTANT, cv::Scalar::all(0));
	//copyMakeBorder(down_image, down_image, 0, 0, 3, 3, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	Point dst_up, dst_down;

	GetCenterCirPoint(up_image, 0.25, 100000, 150000, dst_up);
	GetCenterCirPoint(down_image, 0.25, 100000, 150000, dst_down);
	//GetCenterCirPoint(up_image, 0.1, 10000, 50000, dst_up);
	//GetCenterCirPoint(down_image, 0.1, 10000, 50000, dst_down);
	//cout << dst_up << "  " << dst_down << endl;

	int dst_width = up_image.cols + 2 * abs(dst_up.x - dst_down.x);  //取最右点的长度为拼接图的长度
	int dst_height = dst_up.y + down_image.rows - dst_down.y;
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);
	//cout << "原图的尺寸：" << up_image.cols << " " << up_image.rows << endl;
	//cout << "dst_up的尺寸：" << dst_up.x << " " << dst_up.y << endl;
	Mat up = up_image(Rect(0, 0, up_image.cols, dst_up.y));
	Mat down = down_image(Rect(Point(0, dst_down.y), Point(down_image.cols, down_image.rows)));
	up.copyTo(dst(Rect(abs(dst_up.x - dst_down.x), 0, up_image.cols, dst_up.y)));
	if (dst_up.x > dst_down.x)//上左下右
	{
		down.copyTo(dst(Rect(2 * abs(dst_up.x - dst_down.x), dst_up.y, down.cols, down.rows)));
	}
	else
	{
		down.copyTo(dst(Rect(0, dst_up.y, down.cols, down.rows)));
	}
	output = dst;
	return 0;
}

int GetLocationsOfDamagedLEDs(Mat image, vector<deque<Point>>& template_, int* x, int* y)
{
	Mat smaller;
	Scalar mean;
	Scalar stddev;
	resize(image, smaller, Size(), 0.25, 0.25, INTER_AREA);
	Mat area;
	int length = 0;
	for (size_t i = 0; i < template_.size(); i++)
	{
		for (size_t j = 0; j < template_[i].size(); j++)
		{
			//生成矩形框
			Rect rect = Rect(Point(template_[i][j].x - 3, template_[i][j].y - 3), Point(template_[i][j].x + 3, template_[i][j].y + 3));
			area = smaller(rect);
			meanStdDev(area, mean, stddev);//计算平均值和标准差
			if (mean[2] < 100)
			{
				*x = i;
				*y = j;
				x++;
				y++;
				length++;//有多少个缺失
				circle(image, template_[i][j] * 4, 20, Scalar(0, 255, 0), 50, 8, 0);
				//rectangle(image, 4*Point(template_[i][j].x - 5, template_[i][j].y - 5), 4*Point(template_[i][j].x + 5, template_[i][j].y + 5), Scalar(0, 255, 0), 5, 8, 0);
				cout << "第" << i << "行" << "第" << j << "列" << "缺失！！！" << endl;
			}
		}
	}
	
	namedWindow("image", 0);
	imshow("image", image);
	return length;
}

int GetOrderedDistribution(Mat image, vector<deque<Point>>& output)
{
	Mat smaller;
	resize(image, smaller, Size(), 0.25, 0.25, INTER_AREA);
	if (smaller.channels() == 3) { cvtColor(smaller, smaller, COLOR_BGR2GRAY); }//转化边缘检测后的图为灰度图
	threshold(smaller, smaller, 0, 255, THRESH_OTSU);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(smaller, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);
	/*int count_i;
	for (count_i = 0; count_i < 4; count_i++)
	{
		cout << hierarchy[1][count_i] << endl;
	}*/
	double area_value = 0;
	RotatedRect minRects;
	vector<Point> rects_center;
	for (int i = 0; i < contours.size(); i++)
	{
		//cout << contourArea(contours[i]) << "  ";
		area_value = contourArea(contours[i]);
		if ((area_value >= 150) && (area_value < 300))//正常的检测区域
		{

			minRects = minAreaRect(Mat(contours[i]));
			rects_center.push_back(minRects.center);
		}
	}
	cout << "中心点:" << minRects.center << "角度:" << minRects.angle << "大小:" << minRects.size << endl;
	cout << "rects_center.size:" << rects_center.size() << endl;
	sort(rects_center.begin(), rects_center.end(), [](const Point& a, const Point& b) {return a.y < b.y; });
	//for_each(rects_center.begin(), rects_center.end(), [](const Point& pt) {cout << pt << endl; });
	deque<Point> row;
	int row_number = 0;
	size_t i = 0;
	while (i+1 < rects_center.size())
	{
	if (abs(rects_center[i].y - rects_center[i + 1].y) < 20)
		{
			row.push_front(rects_center[i]);
			i++;
		}
		else
		{
			row.push_front(rects_center[i]);
			output.push_back(row);
			row.clear();
			i++;
		}
	}
	//cout << "总计：" << rows.size() << endl;
	for (size_t i = 0; i < output.size(); i++)
	{
		size_t end = output[i].size();
		//cout << "第" << i << "行的数量：" << rows[i].size() << endl;;
		sort(output[i].begin(), output[i].end(), [](const Point& a, const Point& b) {return a.x < b.x; });
	}
	//for (size_t i = 0; i < output.size(); i++)
	//{
	//	size_t end = output[i].size();
	//	//cout << "第" << i << "行的数量：" << rows[i].size() << endl;;
	//	sort(output[i].begin(), output[i].end(), [](const Point& a, const Point& b) {return a.x < b.x; });
	//}
	return 0;
}
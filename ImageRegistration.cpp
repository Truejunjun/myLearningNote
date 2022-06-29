#include "ImageRegistration.h"

int GetCenterCirPoint(Mat image, float f_xy,int min_area,int max_area, Point& pt)
{
	//Mat smaller;
	resize(image, image, Size(), f_xy, f_xy,INTER_AREA);
	//Mat smaller_gray;
	if (image.channels() == 3) { cvtColor(image, image, COLOR_BGR2GRAY); }//ת����Ե�����ͼΪ�Ҷ�ͼ
	medianBlur(image, image, 5);
	threshold(image, image, 0, 255, THRESH_OTSU);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(image, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);
	int area_value = 0;
	Point2f center;//��Χ�㼯����СԲ��vector
	float radius = 0;//��Χ�㼯����СԲ�ΰ뾶vector
	//vector<vector<Point>> ExistContours;//ɸѡĿ���������
	for (int i = 0; i < contours.size(); i++)
	{
		//cout << contourArea(contours[i]) << "  ";
		area_value = contourArea(contours[i]);
		if ((area_value >= min_area) && (area_value < max_area))//�����ļ������
		{
			minEnclosingCircle(Mat(contours[i]), center, radius);//���ڸ��㼯���ϣ�Ѱ����С����İ�ΧԲ�Σ� //���㲢���ذ�Χ�����㼯����СԲ�μ���뾶
			pt = Point(center.x * (1.0/ f_xy), center.y * (1.0 / f_xy));												//circle(img, Point(center[i].x+ 355,center[i].y+1366), radius[i], Scalar(0, 0, 255), 2, 8, 0);//����Ѱ�ҵ�Բ�Σ�
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

	int dst_width = up_image.cols + 2 * abs(dst_up.x - dst_down.x);  //ȡ���ҵ�ĳ���Ϊƴ��ͼ�ĳ���
	int dst_height = dst_up.y + down_image.rows - dst_down.y;
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);
	//cout << "ԭͼ�ĳߴ磺" << up_image.cols << " " << up_image.rows << endl;
	//cout << "dst_up�ĳߴ磺" << dst_up.x << " " << dst_up.y << endl;
	Mat up = up_image(Rect(0, 0, up_image.cols, dst_up.y));
	Mat down = down_image(Rect(Point(0, dst_down.y), Point(down_image.cols, down_image.rows)));
	up.copyTo(dst(Rect(abs(dst_up.x - dst_down.x), 0, up_image.cols, dst_up.y)));
	if (dst_up.x > dst_down.x)//��������
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
			//���ɾ��ο�
			Rect rect = Rect(Point(template_[i][j].x - 3, template_[i][j].y - 3), Point(template_[i][j].x + 3, template_[i][j].y + 3));
			area = smaller(rect);
			meanStdDev(area, mean, stddev);//����ƽ��ֵ�ͱ�׼��
			if (mean[2] < 100)
			{
				*x = i;
				*y = j;
				x++;
				y++;
				length++;//�ж��ٸ�ȱʧ
				circle(image, template_[i][j] * 4, 20, Scalar(0, 255, 0), 50, 8, 0);
				//rectangle(image, 4*Point(template_[i][j].x - 5, template_[i][j].y - 5), 4*Point(template_[i][j].x + 5, template_[i][j].y + 5), Scalar(0, 255, 0), 5, 8, 0);
				cout << "��" << i << "��" << "��" << j << "��" << "ȱʧ������" << endl;
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
	if (smaller.channels() == 3) { cvtColor(smaller, smaller, COLOR_BGR2GRAY); }//ת����Ե�����ͼΪ�Ҷ�ͼ
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
		if ((area_value >= 150) && (area_value < 300))//�����ļ������
		{

			minRects = minAreaRect(Mat(contours[i]));
			rects_center.push_back(minRects.center);
		}
	}
	cout << "���ĵ�:" << minRects.center << "�Ƕ�:" << minRects.angle << "��С:" << minRects.size << endl;
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
	//cout << "�ܼƣ�" << rows.size() << endl;
	for (size_t i = 0; i < output.size(); i++)
	{
		size_t end = output[i].size();
		//cout << "��" << i << "�е�������" << rows[i].size() << endl;;
		sort(output[i].begin(), output[i].end(), [](const Point& a, const Point& b) {return a.x < b.x; });
	}
	//for (size_t i = 0; i < output.size(); i++)
	//{
	//	size_t end = output[i].size();
	//	//cout << "��" << i << "�е�������" << rows[i].size() << endl;;
	//	sort(output[i].begin(), output[i].end(), [](const Point& a, const Point& b) {return a.x < b.x; });
	//}
	return 0;
}
#pragma once
#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;
//����:�õ�Բ�ĵ����ꣻ
//����image������ͼƬ��
//����f_xy������ϵ����
//����min_area��Բ����С�����
//����max_area��Բ����������
//���pt��Բ�����ꣻ
//����ֵ���ɹ�Ϊ0��
int GetCenterCirPoint(Mat image, float f_xy, int min_area, int max_area, Point& pt);

//����������ͼƬ���ϲ��ֺ�ͼƬ���²��֣�����ƴ�ӳ���ɵ�ͼƬ��
//����up_image��ͼƬ�ϲ��֣�
//����down_image��ͼƬ�²��֣�
//���output��ƴ�Ӻ��ͼƬ��
//����ֵ���ɹ�����0��
int GetStitchedImage(Mat up_image, Mat down_image, Mat& output);

//��������ģ��ͼƬ�еõ�LEDоƬ�ĵ�λ����Ϣ��
//����image������ģ��ͼƬ��
//���output���õ�оƬ������ֲ�ͼ�������ң����ϵ��£�
//����ֵ���ɹ�����0��
int GetOrderedDistribution(Mat image, vector<deque<Point>>& output);

//����������ģ��õ���λ����Ϣ���Ӵ����ͼƬ���ҵ�δ����LED��λ����Ϣ��
//����image����������ͼƬ��
//����template_����ģ��õ���оƬ�ֲ�λ����Ϣ��
//���x��δ����LED�ĺ�����ļ��ϣ�
//���y��δ����LED��������ļ��ϣ�
//����ֵ������δ����LED��������
int GetLocationsOfDamagedLEDs(Mat image, vector<deque<Point>>& template_, int* x, int* y);

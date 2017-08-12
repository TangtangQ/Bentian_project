// siftTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <opencv2/features2d/features2d.hpp>  
#include<opencv2/nonfree/nonfree.hpp>  
#include<opencv2/legacy/legacy.hpp>  
#include<vector>  
#include <opencv/cv.h>  
#include <opencv/highgui.h>  
#include <iostream>  
#include<math.h>

using namespace std;
using namespace cv;

Mat src, frameImg;
int width;
int height;
vector<Point> srcCorner(4);
vector<Point> dstCorner(4);

Mat preProcess(Mat img)
{
	float scale = 0.25;
	Mat smallimg;
	resize(img, smallimg, Size(img.cols * scale, img.rows * scale), 0, 0, INTER_LINEAR);
	return smallimg;
}


struct TZ
{
	vector < CvPoint > Realkp;
	float Avgdis ;//----计算到矫正点的平均距离-----
	//int Offset ;//偏值
	CvPoint Roimedpoint;//感兴趣区中心点

};


//-------说明  照片中尽量只有机器，没有其他杂物，否则其他杂物也将作为特征，会影响最后结果
int _tmain(int argc, _TCHAR* argv[])
{
	Mat orig = imread("C:/Users/tq/Desktop/本田/本田/空滤器螺母/有/1.jpg");
	Mat img;
	img = preProcess(orig);
	
	initModule_nonfree();
	Ptr<FeatureDetector> detector = FeatureDetector::create("SIFT");
	Ptr<DescriptorExtractor> descriptor_extractor = DescriptorExtractor::create("SIFT");
	Ptr<DescriptorMatcher> descriptor_matcher = DescriptorMatcher::create("BruteForce");
	if (detector.empty() || descriptor_extractor.empty())
		throw runtime_error("fail to create detector!");

	vector<KeyPoint> keypoints;
	detector->detect(img, keypoints);
	Mat descriptors;
	descriptor_extractor->compute(img, keypoints, descriptors);
	Mat img_keypoints;
	drawKeypoints(img, keypoints, img_keypoints, Scalar::all(-1), 0);
	descriptor_extractor->compute(img, keypoints, descriptors);
	//---------------realkp--------------
	vector<CvPoint > kp;//将keypoint转化成cvpoint存入kp中
	vector<CvPoint > realkp;//将kp中找到有用的特征点
	for (int index = 0; index < keypoints.size(); index++)
	{
		kp.push_back(cvPoint(keypoints[index].pt.x, keypoints[index].pt.y));
	}
	//----------以图片中心为一个校准点---离得太远的点则去掉--------
	Point2f picmedpoint;
	picmedpoint.x = img.cols*0.5;
	picmedpoint.y = img.cols*0.5;
	//----计算到矫正点的平均距离-----
	float avgdis = 0;
	for (int index = 0; index < kp.size(); index++)
	{
		float d = 0;
		d = (picmedpoint.x - kp[index].x)*(picmedpoint.x - kp[index].x) + (picmedpoint.y - kp[index].y)*(picmedpoint.y - kp[index].y);
		if (index == 0)
		{
			avgdis = sqrt(d);
		}
		else
			avgdis = (avgdis + sqrt(d)) / 2;
	}
	int offset = 50;
	CvPoint roimedpoint = cvPoint(0, 0);
	for (int i = 0; i < kp.size(); i++)
	{
		float d = 0;
		d = (picmedpoint.x - kp[i].x)*(picmedpoint.x - kp[i].x) + (picmedpoint.y - kp[i].y)*(picmedpoint.y - kp[i].y);
		int tempcz = abs(sqrt(d) - avgdis);
		if (tempcz <= offset)
		{
			//circle(img1, kp[i], 5, CV_RGB(255, 0, 0));
			realkp.push_back(kp[i]);//找有用的kp
			if (roimedpoint.x == 0)
			{
				roimedpoint.x = kp[i].x;
				roimedpoint.y = kp[i].y;
			}
			else
			{
				roimedpoint.x = (roimedpoint.x + kp[i].x) / 2;
				roimedpoint.y = (roimedpoint.y + kp[i].y) / 2;
			}
		}
	}
	//circle(img1, roimedpoint, avgdis, CV_RGB(255, 0, 0));
	//rectangle(myImg, cvPoint(myTz.Roimedpoint.x - myTz.Avgdis - myoffset * 3, myTz.Roimedpoint.y - myTz.Avgdis - myoffset), cvPoint(myTz.Roimedpoint.x + myoffset + myTz.Avgdis, myTz.Roimedpoint.y + myoffset + myTz.Avgdis), CV_RGB(255, 0, 0), 3);
	circle(img, roimedpoint, 5, CV_RGB(255, 0, 0),8);
	rectangle(img, cvPoint(roimedpoint.x - avgdis - offset, roimedpoint.y - avgdis - offset), cvPoint(roimedpoint.x + offset + avgdis, roimedpoint.y + offset + avgdis), CV_RGB(255, 0, 0), 3);
	imshow("kp", img);
	waitKey(0);
	return 0;
}
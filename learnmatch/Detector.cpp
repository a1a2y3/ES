/* 
 * Code to accompany the paper:
 *   Efficient Online Structured Output Learning for Keypoint-Based Object Tracking
 *   Sam Hare, Amir Saffari, Philip H. S. Torr
 *   Computer Vision and Pattern Recognition (CVPR), 2012
 * 
 * Copyright (C) 2012 Sam Hare, Oxford Brookes University, Oxford, UK
 * 
 * This file is part of learnmatch.
 * 
 * learnmatch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * learnmatch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with learnmatch.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */
#include "stdafx.h"
#include "Detector.h"
#include "Config.h"
#include "Model.h"

using namespace std;
using namespace cv;
using namespace Eigen;

Detector::Detector(const Config& config) :
	m_config(config),
	m_pModel(0),
	m_pLogger(0)
{
}


Detector::~Detector()
{
	Reset();
}


void Detector::Reset()
{
	if (m_pModel)
	{
		delete m_pModel;
		m_pModel = 0;
	}
	if (m_pLogger)
	{
		delete m_pLogger;
		m_pLogger = 0;
	}
	
	m_useClassifiers = m_config.enableLearning;
	m_useBinaryWeightVector = m_config.enableBinaryWeightVector;
}


bool Detector::SetModel(const Mat& image, const IntRect& rect)
{	
	Reset();
	
	if (m_config.doLog)
	{
		m_pLogger = new StatsTextLogger(m_config.logPath);
	}
	m_pModel = new Model(m_config, image, rect);
	cout << "initialised detector" << endl;
	
	return true;
}

void Detector::Detect(const cv::Mat& image, bool doUpdate)
{
	m_pModel->m_useClassifiers = m_useClassifiers;
	m_pModel->m_useBinaryWeightVector = m_useBinaryWeightVector;
	m_stats = Stats();
	m_hasDetection = m_pModel->Detect(image, m_H, doUpdate, &m_stats);
}

void Detector::Debug()
{
}
Point2f Detector::projectPoint(Point2f p0, Mat h)
{
	std::vector<Point2f> pts;
	pts.push_back(p0);

	std::vector<Point2f> ptsProj(1);
	Mat C(pts);
	Mat Cproj(ptsProj);
	perspectiveTransform(C, Cproj, h);
	return ptsProj.at(0);
}
float Detector::homoDis(Point2f p0, Point2f p1, Mat h)
{
	std::vector<Point2f> pts;
	pts.push_back(p0);
	pts.push_back(p1);

	std::vector<Point2f> ptsProj(2);
	Mat C(pts);
	Mat Cproj(ptsProj);
	// 角点投影坐标
	perspectiveTransform(C, Cproj, h);
	float dx= ptsProj.at(0).x-ptsProj.at(1).x;
	float dy= ptsProj.at(0).y-ptsProj.at(1).y;
	return sqrt(dx*dx+dy*dy);
}
float areaFromCorner(vector<Point2f> corners)
{
	if (corners.size()<4)
	{
		return 0;
	}
	float dx,dy;
	float a01,a12,a23,a30,a02;
	float p012,p230;
	float s012,s230;
	//        a0          a1
	//         ------------
	//         |          |
	//         |          |
	//         ------------
	//        a3           a2
	// a0--a1
	dx= corners[0].x-corners[1].x;
	dy= corners[0].y-corners[1].y;
	a01= sqrt(dx*dx+dy*dy);
	// a1--a2
	dx= corners[2].x-corners[1].x;
	dy= corners[2].y-corners[1].y;
	a12= sqrt(dx*dx+dy*dy);
	// a2--a3
	dx= corners[2].x-corners[3].x;
	dy= corners[2].y-corners[3].y;
	a23= sqrt(dx*dx+dy*dy);
	// a3--a0
	dx= corners[0].x-corners[3].x;
	dy= corners[0].y-corners[3].y;
	a30= sqrt(dx*dx+dy*dy);
	// a0--a2
	dx= corners[0].x-corners[2].x;
	dy= corners[0].y-corners[2].y;
	a02= sqrt(dx*dx+dy*dy);
	p012= 0.5*(a01*a12+a12*a02+a02*a01);
	p230= 0.5*(a23*a30+a30*a02+a02*a23);
	s012= sqrt(p012*(p012-a01)*(p012-a12)*(p012-a02));
	s230= sqrt(p230*(p230-a23)*(p230-a30)*(p230-a02));
	return s012+s230;
}
float Detector::validAreaRatio(const cv::Mat image, float wRect,float hRect)
{
	std::vector<Point2f> corners;
	corners.push_back(Point2f(0.f, 0.f));
	corners.push_back(Point2f(wRect, 0.f));
	corners.push_back(Point2f(wRect, hRect));
	corners.push_back(Point2f(0.f, hRect));

	std::vector<Point2f> cornersProj(4);
	Mat C(corners);
	Mat Cproj(cornersProj);
	// 角点投影坐标
	perspectiveTransform(C, Cproj, m_H);

	// 角点边界判断
	std::vector<Point2f> cornersValid(4);
	int w= image.cols;
	int h= image.rows;
	for (int i=0;i<4;i++)
	{
		int x= cornersProj[i].x;
		int y= cornersProj[i].y;
		x= x>=0? x:0;
		x= x<=(w-1)? x:(w-1);
		y= y>=0? y:0;
		y= y<=(h-1)? y:(h-1);
		cornersValid[i].x= x;
		cornersValid[i].y= y;
	}
	float s1,s2;
	s1= areaFromCorner(cornersProj);
	s2= areaFromCorner(cornersValid);
	return s1>0? s2/s1:0;
}
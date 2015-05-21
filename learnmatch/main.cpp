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

#include <iostream>
#include <fstream>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Config.h"
#include "Detector.h"
#include "Rect.h"
#include "Video.h"
#include "Utils.h"

using namespace std;
using namespace cv;

static const char* kWindowName = "learnmatch";
static const char* kRecordPath = "D:/data/learnmatch/video";
int main()
{
	Config conf= Config("config.txt");  //read config file
	cout << "config:" << endl;
	cout << conf << endl;

	srand(conf.seed);        // init rand seed
	Detector detector(conf); // init detector

	

	// read first frame to get size
	vector<float> sigVec;
	float sigMul=1.0f;

	Mat frame, frameGrey;
	bool doFlip = false;
	if (!conf.quietMode)
	{
		namedWindow(kWindowName);
		cvMoveWindow(kWindowName, 640, 20);
	}
	for (int frameIdx = 1;;++frameIdx)
	{		
		char imgPath[256];
		string imgFormat = conf.imagePath+"%04d.jpg";
		sprintf(imgPath, imgFormat.c_str(), frameIdx);
		Mat frameGrey = cv::imread(imgPath, 0);
		if (frameGrey.empty()) break;                // end of series
		cvtColor(frameGrey, frame, CV_GRAY2RGB);     // frameGrey: process image
													//  frame : display image
		IntRect rect(frameGrey.cols/3, frameGrey.rows/3, frameGrey.cols/3, frameGrey.rows/3);
		if (!detector.HasModel())     // init
		{
			detector.SetModel(frameGrey, rect);
		}
		//if (pVideoOutput)
		//{
		//	bool ok = pVideoOutput->WriteFrame(frame);
		//	if (!ok) cout << "error writing video frame" << endl;
		//}

		//if (pVideoInput && !detector.HasModel())
		//{
		//	detector.SetModel(frameGrey, rect);
		//}

		if (detector.HasModel())
		{
			detector.Detect(frameGrey, conf.enableLearning);

	
			if (detector.HasDetection())
			{
				DrawHomography(detector.GetH(), frame, rect.Width(), rect.Height(), detector.HasDetection() ? CV_RGB(0, 0, 255) : CV_RGB(255, 0, 0));
				Mat h_mat= detector.GetH();
//				cout<< h_mat;
				int ctx= int(rect.XCentre()+0.5);
				int cty= int(rect.YCentre()+0.5);
				line(frame,Point2i(ctx-10,cty),Point2i(ctx+10,cty),CV_RGB(0, 255, 0));
				line(frame,Point2i(ctx,cty-10),Point2i(ctx,cty+10),CV_RGB(0, 255, 0));
				double a[4];
				a[0]= h_mat.at<double>(0,0);
				a[1]= h_mat.at<double>(0,1);
				a[2]= h_mat.at<double>(1,0);
				a[3]= h_mat.at<double>(1,1);				
				float sigma= sqrt((a[0]*a[0]+a[1]*a[1]+a[2]*a[2]+a[3]*a[3])/2);
				float areaRatio= detector.validAreaRatio(frameGrey,rect.Width(), rect.Height());
				sigVec.push_back(sigMul*sigma);
				cout<<"sigma="<<sigma<<"  sigMul="<<sigMul<<", aR="<<areaRatio<<endl;
				if (sigma>1.7 || sigma<1.0/1.7)  // 如果 H 满足 更新条件
					// 条件1   尺度 sigma> 1.732					
				{
					detector.SetModel(frameGrey, rect);  // 更新 跟踪 区域
					sigMul= sigMul*sigma;
					cout<< "trigger case 1 in frame"<< frameIdx<< endl;
					cout<<h_mat<<endl;
					waitKey(0);
					waitKey(0);
				}				
				if (areaRatio<0.4)  // 如果 H 满足 更新条件
					// 条件2   矩形在图像内面积与矩形总面积比值 小于 0.7 
				{
					detector.SetModel(frameGrey, rect);  // 更新 跟踪 区域
					sigMul= sigMul*sigma;
					cout<< "trigger case 2 in frame"<< frameIdx<< " area ratio"<<areaRatio<< endl;
					waitKey(0);
					waitKey(0);
				}
			}
			detector.Debug();
			
		}
		else
		{
			rectangle(frame, Point2i(rect.XMin(), rect.YMin()), Point2i(rect.XMax(), rect.YMax()), CV_RGB(0, 255, 0), 2);
		}


		//if (pVideoResults)
		//{
		//	bool ok = pVideoResults->WriteFrame(detector.HasModel() ? detector.GetDebugImage() : frame);
		//	if (!ok) cout << "error writing results video frame" << endl;
		//}

		if (!conf.quietMode)
		{
			imshow(kWindowName, detector.HasModel() ? detector.GetDebugImage() : frame);

			imshow("homography", frame);

			int key = waitKey(0);
			if (key != -1)
			{
				if (key == 27 || key == 'q') // 27 = esc     
				{
					break;
				}
				else if (key == 'f')   // flip cam image
				{
					doFlip = !doFlip;
				}
				else if (key == 'i')    // set model
				{
					if (!detector.HasModel())
					{
						detector.SetModel(frameGrey, rect);
					}
					else
					{
						detector.Reset();
					}
				}
				else if (key == 'c')        // 
				{
					detector.m_useClassifiers = !detector.m_useClassifiers;
					cout << "use classifier: " << detector.m_useClassifiers << endl;
				}
				else if (key == 'b')
				{
					detector.m_useBinaryWeightVector = !detector.m_useBinaryWeightVector;
					cout << "use binary weight vector: " << detector.m_useBinaryWeightVector << endl;
				}
				else if (key == 'p')
				{
					waitKey(0);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}
int main_old(int argc, char* argv[])
{
	Config conf;
	IntRect rect(100,100,150,150);
	VideoCapture cap;
	Video* pVideoInput = 0;
	Video* pVideoResults = 0;
	vector<Mat> groundTruth;
	
	bool useVideo = false;
	bool configRead = false;
	for (int i = 1; i < argc; ++i)
	{
 		string arg(argv[i]);
		if (arg == "--config")
		{
			// read config file
			conf = Config(string(argv[++i]));
			configRead = true;
		}
		else if (arg == "--output")
		{
			// save results video
			pVideoResults = new Video(string(argv[++i]), true);
		}
		else
		{
			cout << "unrecognised option: " << arg << endl;
			return EXIT_FAILURE;
		}
	}
	
	if (!configRead)
	{
		// no config path specified, so use default location
		conf = Config("config.txt");
	}
	
	cout << "config:" << endl;
	cout << conf << endl;
	
	srand(conf.seed);
	
	Detector detector(conf);
	
	if (conf.useVideo)
	{
		// run from a video
		pVideoInput = new Video(conf.videoPath, false, "%05d.jpg");
		// read groundtruth
		ifstream gtInit((conf.videoPath+"/gt_init.txt").c_str());
		if (gtInit.eof() || gtInit.bad() || gtInit.fail())
		{
			cout << "failed to open gt_init.txt" << endl;
			return EXIT_FAILURE;
		}
		float x, y, w, h;
		gtInit >> x >> y >> w >> h;
		rect = IntRect(x, y, w, h);
		
		ifstream gtHomography((conf.videoPath+"/gt_homography.txt").c_str());
		if (gtInit.eof() || gtInit.bad() || gtInit.fail())
		{
			cout << "failed to open gt_homograhy.txt" << endl;
			return EXIT_FAILURE;
		}
		while (!(gtHomography.eof() || gtHomography.bad() || gtHomography.fail()))
		{
			Mat H(3, 3, CV_64FC1);
			double* p = H.ptr<double>();
			gtHomography >> p[0] >> p[1] >> p[2] >> p[3] >> p[4] >> p[5] >> p[6] >> p[7] >> p[8];
			if (!(gtHomography.eof() || gtHomography.bad() || gtHomography.fail()))
			{
				groundTruth.push_back(H);
			}
		}
		cout << "groundtruth: " << groundTruth.size() << endl;
	}
	
	if (!pVideoInput)
	{
		cap.open(0);
		if (!cap.isOpened())
		{
			cout << "could not open capture device" << endl;
			return EXIT_FAILURE;
		}
		cap.set(CV_CAP_PROP_FPS, 30.0);
		cap.set(CV_CAP_PROP_FRAME_WIDTH , 640.0);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480.0);
	}
	
	Mat frame, frameGrey;
	Video* pVideoOutput = 0;
	bool doFlip = false;
	if (!conf.quietMode)
	{
		namedWindow(kWindowName);
		cvMoveWindow(kWindowName, 640, 20);
	}
	for (int frameIdx = 0;;++frameIdx)
	{				
		if (!pVideoInput)
		{
			cap.grab();
			cap.retrieve(frame);
			if (doFlip) flip(frame, frame, 1);
			cvtColor(frame, frameGrey, CV_RGB2GRAY);
		}
		else
		{
			bool ok = pVideoInput->ReadFrame(frameGrey);
			if (!ok) break;
			cvtColor(frameGrey, frame, CV_GRAY2RGB);
			DrawHomography(groundTruth[frameIdx], frame, rect.Width(), rect.Height(), CV_RGB(0, 255, 0));
		}

		if (pVideoOutput)
		{
			bool ok = pVideoOutput->WriteFrame(frame);
			if (!ok) cout << "error writing video frame" << endl;
		}
		
		if (pVideoInput && !detector.HasModel())
		{
			detector.SetModel(frameGrey, rect);
		}
		
		if (detector.HasModel())
		{
			detector.Detect(frameGrey, conf.enableLearning);
			
			if (pVideoInput)
			{
				const Mat& Hgt = groundTruth[frameIdx];
				const Mat& H = detector.GetH();
				
				if (detector.HasDetection())
				{
					detector.m_stats.error = HomographyLoss(Hgt, H);
				}
				else
				{
					detector.m_stats.error = 1e8;
				}
				detector.LogStats();
			}
			
			if (detector.HasDetection())
			{
				DrawHomography(detector.GetH(), frame, rect.Width(), rect.Height(), detector.HasDetection() ? CV_RGB(0, 0, 255) : CV_RGB(255, 0, 0));
				int ctx= int(rect.XCentre()+0.5);
				int cty= int(rect.YCentre()+0.5);
				line(frame,Point2i(ctx-10,cty),Point2i(ctx+10,cty),CV_RGB(0, 255, 0));
				line(frame,Point2i(ctx,cty-10),Point2i(ctx,cty+10),CV_RGB(0, 255, 0));
			}
			
			detector.Debug();
		}
		else
		{
			rectangle(frame, Point2i(rect.XMin(), rect.YMin()), Point2i(rect.XMax(), rect.YMax()), CV_RGB(0, 255, 0), 2);
			
		}
		
		
		if (pVideoResults)
		{
			bool ok = pVideoResults->WriteFrame(detector.HasModel() ? detector.GetDebugImage() : frame);
			if (!ok) cout << "error writing results video frame" << endl;
		}
		
		if (!conf.quietMode)
		{
			imshow(kWindowName, detector.HasModel() ? detector.GetDebugImage() : frame);
			
			imshow("homography", frame);
			
			int key = waitKey(1);
			if (key != -1)
			{
				if (key == 27 || key == 'q') // 27 = esc
				{
					break;
				}
				else if (key == 'f')
				{
					doFlip = !doFlip;
				}
				else if (key == 'i')
				{
					if (!detector.HasModel())
					{
						detector.SetModel(frameGrey, rect);
					}
					else
					{
						detector.Reset();
					}
				}
				else if (key == 'c')
				{
					detector.m_useClassifiers = !detector.m_useClassifiers;
					cout << "use classifier: " << detector.m_useClassifiers << endl;
				}
				else if (key == 'b')
				{
					detector.m_useBinaryWeightVector = !detector.m_useBinaryWeightVector;
					cout << "use binary weight vector: " << detector.m_useBinaryWeightVector << endl;
				}
				else if (key == 'p')
				{
					waitKey();
				}
				else if (key == 'r' && !pVideoInput)
				{
					if (!pVideoOutput)
					{
						pVideoOutput = new Video(kRecordPath, true);
					}
					else
					{
						delete pVideoOutput;
						pVideoOutput = 0;
					}
				}
			}
		}
	}
	
	if (pVideoInput) delete pVideoInput;
	if (pVideoOutput) delete pVideoOutput;
	if (pVideoResults) delete pVideoResults;
	return EXIT_SUCCESS;
}

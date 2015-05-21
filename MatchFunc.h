#pragma once

#include <opencv\cv.h>
#include <opencv\highgui.h>
// “Ï‘¥ÕºœÒ∆•≈‰
double lz_Multi_Sensor_MatchTemplate(IplImage *pLargeImg, IplImage *pSmallImg, CvPoint* pointInLarge, 
								   int step=1, CvPoint smTarget= cvPoint(0,0), CvRect targetRect=cvRect(0,0,0,0));

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
#include "Config.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <cassert>

using namespace std;

Config::Config(const std::string& path)
{
	SetDefaults();
	ifstream f(path.c_str());
	if (!f)
	{
		cout << "error: could not load config file: " << path << endl;
		return;
	}

	string line, name, tmp;
	while (getline(f, line))
	{
		istringstream iss(line);
		iss >> name >> tmp;

		// skip invalid lines and comments
		if (iss.fail() || tmp != "=" || name[0] == '#') continue;
		if (name == "seed")
		{
			iss>> seed;
		}
		else if (name == "quietMode")
		{
			iss>> quietMode;
		}
		else if (name == "useVideo")
		{
			iss>> useVideo;
		}
		else if (name == "videoPath")
		{
			iss>> videoPath;
		}
		else if (name == "imagePath")
		{
			iss>> imagePath;
		}
		else if (name == "doLog")
		{
			iss>> doLog;
		}
		else if (name == "logPath")
		{
			iss>> logPath;
		}
		else if (name == "maxModelKeypoints")
		{
			iss>> maxModelKeypoints;
		}
		else if (name == "maxDetectKeypoints")
		{
			iss>> maxDetectKeypoints;
		}
		else if (name == "prosacIts")
		{
			iss>> prosacIts;
		}
		else if (name == "svmLambda")
		{
			iss>> svmLambda;
		}
		else if (name == "svmNu")
		{
			iss>> svmNu;
		}					
		else if (name == "svmBinaryComponents")
		{
			iss>> svmBinaryComponents;
		}
		else if (name == "enableLearning")
		{
			iss>> enableLearning;
		}
		else if (name == "enableBinaryWeightVector")
		{
			iss>> enableBinaryWeightVector;
		}
		else if (name == "lossFunction")
		{
			string lf;
			iss>> lf;
			if (lf == Config::LossFunctionName(kLossFunctionNumInliers))
			{
				lossFunction = kLossFunctionNumInliers;
			}
			else if (lf == Config::LossFunctionName(kLossFunctionHomography))
			{
				lossFunction = kLossFunctionHomography;
			}
			else if (lf == Config::LossFunctionName(kLossFunctionHamming))
			{
				lossFunction = kLossFunctionHamming;
			}
			else if (lf == Config::LossFunctionName(kLossFunctionNone))
			{
				lossFunction = kLossFunctionNone;
			}
			else
			{
				cout << "unrecognised loss function: " << lf << endl;
				cout << "valid options are: (" 
					<< Config::LossFunctionName(kLossFunctionNumInliers) << ","
					<< Config::LossFunctionName(kLossFunctionHamming) << ","
					<< Config::LossFunctionName(kLossFunctionHomography) << ","
					<< Config::LossFunctionName(kLossFunctionNone) << ")" << endl;
				assert(false);
			}
		}
	}

}

void Config::SetDefaults()
{
	seed = 0;
	quietMode = false;
	useVideo = false;
	videoPath = "";
	imagePath = "";
	doLog = false;
	logPath = "log.txt";
	lossFunction = kLossFunctionNumInliers;
	maxModelKeypoints = 100;
	maxDetectKeypoints = 1000;
	maxMatchesPerModelKeypoint = 2; // hard-wired now
	prosacIts = 256;
	svmLambda = 0.1;
	svmNu = 1.0;
	svmBinaryComponents = 2;
	enableLearning = true;
	enableBinaryWeightVector = true;
}

string Config::LossFunctionName(LossFunction lf)
{
	string s;
	switch (lf)
	{
	case kLossFunctionNone:
		s = "none";
		break;	
	case kLossFunctionNumInliers:
		s = "numInliers";
		break;
	case kLossFunctionHomography:
		s = "homography";
		break;
	case kLossFunctionHamming:
		s = "hamming";
		break;
	default:
		assert(false);
	}
	return s;
}

ostream& operator<< (ostream& out, const Config& conf)
{
    out << "  seed                       = " << conf.seed << endl;
	out << "  quietMode                  = " << conf.quietMode << endl;
	out << "  useVideo                   = " << conf.useVideo << endl;
	out << "  videoPath                  = " << conf.videoPath << endl;
	out << "  imagePath                  = " << conf.imagePath << endl;
	out << "  doLog                      = " << conf.doLog << endl;
	out << "  logPath                    = " << conf.logPath << endl;
	out << "  lossFunction               = " << Config::LossFunctionName(conf.lossFunction) << endl;
	out << "  maxModelKeypoints          = " << conf.maxModelKeypoints << endl;
	out << "  maxDetectKeypoints         = " << conf.maxDetectKeypoints << endl;
	out << "  maxMatchesPerModelKeypoint = " << conf.maxMatchesPerModelKeypoint << endl;
	out << "  prosacIts                  = " << conf.prosacIts << endl;
	out << "  svmLambda                  = " << conf.svmLambda << endl;
	out << "  svmNu                      = " << conf.svmNu << endl;
	out << "  svmBinaryComponents        = " << conf.svmBinaryComponents << endl;
	out << "  enableLearning             = " << conf.enableLearning << endl;
	out << "  enableBinaryWeightVector   = " << conf.enableBinaryWeightVector << endl;
	return out;
}
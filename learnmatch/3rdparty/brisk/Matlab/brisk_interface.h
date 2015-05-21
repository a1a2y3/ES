/*=================================================================
 *
 *  BRISK - Binary Robust Invariant Scalable Keypoints
 *  Reference implementation of
 *  [1] Stefan Leutenegger,Margarita Chli and Roland Siegwart, BRISK:
 *  	Binary Robust Invariant Scalable Keypoints, in Proceedings of
 *  	the IEEE International Conference on Computer Vision (ICCV2011).
 *
 *  Copyright (C) 2011  The Autonomous Systems Lab, ETH Zurich,
 *  Stefan Leutenegger and Margarita Chli.
 *
 *  This file is part of BRISK.
 *
 *  BRISK is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  BRISK is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with BRISK.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=================================================================*/

#include <brisk/brisk.h>
#include <mex.h>
#include <vector>
#include <string.h>
#include <opencv2/opencv.hpp>

// helper function to test input args
bool mxIsScalarNonComplexDouble(const mxArray* arg);

class BriskInterface{
public:
    // constructor - calls set
    BriskInterface( int nlhs, mxArray *plhs[], 
            int nrhs, const mxArray *prhs[] );
    // destructor
    ~BriskInterface();
    
    // reset octaves / threshold / pattern
    inline void set(int nlhs, mxArray *plhs[], 
            int nrhs, const mxArray *prhs[]);
    // load an image
    inline void loadImage( int nlhs, mxArray *plhs[], 
            int nrhs, const mxArray *prhs[] );
    // detection
    inline void detect(int nlhs, mxArray *plhs[], 
            int nrhs, const mxArray *prhs[]);
    // descriptor extraction
    inline void describe(int nlhs, mxArray *plhs[], 
            int nrhs, const mxArray *prhs[]);
    // descriptor matching
    inline void radiusMatch( int nlhs, mxArray *plhs[], 
            int nrhs, const mxArray *prhs[] );
    inline void knnMatch( int nlhs, mxArray *plhs[], 
            int nrhs, const mxArray *prhs[] );
    // grayImage access
    inline void image( int nlhs, mxArray *plhs[], 
            int nrhs, const mxArray *prhs[] );
    
        
private:
    cv::BriskFeatureDetector* p_detector; // detector
    cv::BriskDescriptorExtractor* p_descriptor; // descriptor
    cv::BruteForceMatcher<cv::HammingSse>* p_matcher; // matcher
    cv::Mat img; // temporary image stored with loadImage
    std::vector<cv::KeyPoint> keypoints; // temporary keypoint storage
    
    // settings
    unsigned int threshold;
    unsigned int octaves;
    bool rotationInvariant;
    bool scaleInvariant; 
    float patternScale;
};

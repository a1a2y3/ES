#pragma once

#include "stdio.h"
#include "math.h"

#include "opencv\cv.h"
#include "opencv\highgui.h"

#include "MatchFunc.h"
#include "ImageProcess.h"
#include "FileProcess.h"


#include "ManualAdjustDlg.h"
#include "SelectTargetDlg.h"
#include "ProgressDlg.h"
#include "ConfirmDlg.h"
#include "TargetNumDlg.h"
#include "TargetRectDlg.h"
#include "SetTarPos.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
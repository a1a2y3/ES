% mex library build script
% written by Stefan Leutenegger, 8/2011

% mex build command
clc;
clear all;

% set the openCV path:
windowsOpenCvDir='C:/OpenCV2.2';
unixOpenCvDir='/home/lestefan/ros/vision_opencv/opencv2/opencv';

% determine the build and mex directory
c='';
directory='';
switch computer
    case 'GLNXA64' 
        c='glnxa64';
        directory='unix64';
    case 'GLNX86' 
        c='glnx86';
        directory='unix32';
    case 'PCWIN' 
        c='pcwin';
        directory='win32';
    case 'PCWIN64' 
        c='pcwin64';
    case 'MACI' 
        c='maci';
        directory='apple';
    case 'MACI64' 
        c='maci64';
    otherwise
        disp('error determining architecture');
end

if strcmp(computer,'PCWIN')
    mex('brisk.cpp', ...
    ['-I' windowsOpenCvDir '/include'], ...
	'-I../include', ...
	'-I../thirdparty/agast/include', ... 
    ['-L' windowsOpenCvDir '/lib'], ...
    ['-L' windowsOpenCvDir '/3rdparty/lib'], ...
    '-L../win32/lib/Release', ...
    '-L../win32/lib', ...
    '-L../thirdparty/agast/Release', ...
    '-lbrisk_static', '-lagast_static', ...
    '-lopencv_features2d220', ...
    '-lopencv_imgproc220', ...
    '-lopencv_calib3d220', '-lopencv_highgui220', ...
    '-lopencv_core220', '-lopencv_imgproc220', '-lopencv_flann220');
else
    eval(['mex brisk.cpp -v ',...
    '-I' unixOpenCvDir '/include ' ...
	'-I../include ' ...
	'-I../thirdparty/agast/include ' ... 
    '-L../' directory '/lib '  ...
     '-Wl']);

    % kind of dangerous, but a nice hack:
    fix_error;

end

% copy to correct bin directory
system(['cp brisk.mex* ../' directory '/bin']);
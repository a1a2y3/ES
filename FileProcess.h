#if !defined  _IMGAEPROC_567864_H
#define      _IMGAEPROC_567864_H

BOOL IsExistFile(CString Filename);
//序列图像处理 关于文件名的处理函数
BOOL FindNextFile_L(CString &Filename);
BOOL FindPrevFile_L(CString &Filename);
bool GetFileNumber(CString FileName, int &n);
void      ResetCurrentFileNameFromNumber( CString & FileName , int FileNum);
// 取下帧图像文件名，若失败，返回null
CString   GetNextFileName_1(CString strCurrentFileName);
// 取下帧图像文件名，若失败，则下下帧，若再失败，则下下下帧。连续3帧失败，返回null
CString   GetNextFileName_123(CString strCurrentFileName);
CString   GetPreFileName_1(CString strCurrentFileName);
CString   GetPreFileName_123(CString strCurrentFileName);
void GetFolderPathFromFilePath( CString FileName , CString &FilePath);
void GetFileNameFromFilePath( CString FileName , CString &fname);

#endif

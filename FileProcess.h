#if !defined  _IMGAEPROC_567864_H
#define      _IMGAEPROC_567864_H

BOOL IsExistFile(CString Filename);
//����ͼ���� �����ļ����Ĵ�����
BOOL FindNextFile_L(CString &Filename);
BOOL FindPrevFile_L(CString &Filename);
bool GetFileNumber(CString FileName, int &n);
void      ResetCurrentFileNameFromNumber( CString & FileName , int FileNum);
// ȡ��֡ͼ���ļ�������ʧ�ܣ�����null
CString   GetNextFileName_1(CString strCurrentFileName);
// ȡ��֡ͼ���ļ�������ʧ�ܣ�������֡������ʧ�ܣ���������֡������3֡ʧ�ܣ�����null
CString   GetNextFileName_123(CString strCurrentFileName);
CString   GetPreFileName_1(CString strCurrentFileName);
CString   GetPreFileName_123(CString strCurrentFileName);
void GetFolderPathFromFilePath( CString FileName , CString &FilePath);
void GetFileNameFromFilePath( CString FileName , CString &fname);

#endif

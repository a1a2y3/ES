
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

//#ifndef _SECURE_ATL
//#define _SECURE_ATL 1
//#endif
//
//#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
//#endif
//
//
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的
//
//// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
//#define _AFX_ALL_WARNINGS

//#include <afxwin.h>         // MFC 核心组件和标准组件
//#include <afxext.h>         // MFC 扩展
//#include <afxdisp.h>        // MFC 自动化类
//#ifndef _AFX_NO_OLE_SUPPORT
//#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
//#endif
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
//#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
//#include <afxdhtml.h>



//#ifndef YX_BYTE//8 - bit 1 - channel 
//#define YX_BYTE(img,y,x) ((BYTE*)(img->imageData + (y)*img->widthStep))[x]
//#endif
//
//#ifndef YX_BYTE_B//8 - bit 3 - channel
//#define YX_BYTE_B(img,y,x) ((BYTE*)(img->imageData + (y)*img->widthStep))[x*3]
//#endif
//
//#ifndef YX_BYTE_G//8 - bit 3 - channel
//#define YX_BYTE_G(img,y,x) ((BYTE*)(img->imageData + (y)*img->widthStep))[x*3+1]
//#endif
//
//#ifndef YX_BYTE_R//8 - bit 3 - channel
//#define YX_BYTE_R(img,y,x) ((BYTE*)(img->imageData + (y)*img->widthStep))[x*3+2]
//#endif

void  DelayMessage();




//#ifdef _UNICODE
//#if defined _M_IX86
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_IA64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_X64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
//#endif



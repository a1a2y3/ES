
// stdafx.cpp : 只包括标准包含文件的源文件
// Es.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


//延迟消息
//引用SPD-3中的函数

void DelayMessage() 
{
	MSG message;
	while(::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}

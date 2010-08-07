﻿// YSLib::Service::YShellInitialization by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-10-21 23:15:08;
// UTime = 2010-7-21 8:14;
// Version = 0.1476;


#ifndef INCLUDED_YSINIT_H_
#define INCLUDED_YSINIT_H_

// YShellInitialization ：程序启动时的通用初始化代码段。

#include "../Core/ysdef.h"

YSL_BEGIN

//初始化主控制台。
void
InitYSConsole();


//默认字型路径和目录。
extern CPATH DEF_FONT_PATH;
extern CPATH DEF_FONT_DIRECTORY;

//初始化系统字体缓存：以默认字型路径创建默认字体缓存并加载默认字型目录中的字体文件。
void
InitializeSystemFontCache();

//注销系统字体缓存。
void
DestroySystemFontCache();

void
EpicFail();

void
LibfatFail();

void
CheckInstall();

void
CheckSystemFontCache();

YSL_END

#endif

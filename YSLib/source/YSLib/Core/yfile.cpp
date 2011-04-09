﻿/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfile.cpp
\ingroup Core
\brief 平台无关的文件抽象。
\version 0.1334;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-24 23:14:51 +0800;
\par 修改时间:
	2011-03-07 13:10 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile;
*/


#include "yfile.h"

YSL_BEGIN

YFile::YFile(CPATH p)
	: YObject(),
	fp(NULL), fsize(0)
{
	if(Open(p))
	{
		fseek(0, SEEK_END);
		fsize = ftell();
		rewind();
	}
}

YFile::~YFile()
{
	Release();
}

void
YFile::Release()
{
	if(IsValid())
		std::fclose(fp);
}

bool
YFile::Open(CPATH p)
{
	Release();
	fp = std::fopen(p, "r");
	return IsValid();
}

YSL_END

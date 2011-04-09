﻿/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycomp.cpp
\ingroup Shell
\brief 平台无关的组件和图形用户界面组件实现。
\version 0.1896;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-19 20:05:18 +0800;
\par 修改时间:
	2011-03-05 20:31 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YComponent;
*/


#include "ydesktop.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

void
Activate(YConsole& c, Drawing::Color fc, Drawing::Color bc)
{
	InitConsole(c.Screen, fc, bc);
}

void
Deactivate(YConsole&)
{
	InitVideo();
}

YSL_BEGIN_NAMESPACE(Widgets)


YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

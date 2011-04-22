﻿/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yres.h
\ingroup Core
\brief 应用程序资源管理模块。
\version 0.1488;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:46:40 +0800;
\par 修改时间:
	2011-04-22 21:56 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YResource;
*/


#ifndef YSL_INC_CORE_YRES_H_
#define YSL_INC_CORE_YRES_H_

#include "ycounter.hpp"
#include "ystatic.hpp"
#include "../Shell/ygdi.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

//! \brief 资源接口。
DeclInterface(IResource)
EndDecl


/*!
\ingroup HelperFunction
\brief 全局默认共享资源生成函数。
\note 线程空间内共享；全局资源初始化之后可调用。
*/
template<class _type>
inline GHWeak<_type>
GetGlobalResource()
{
	return GStaticCache<_type, GHandle<_type> >::GetPointer();
}

/*!
\ingroup HelperFunction
\brief 全局默认共享资源释放函数。
*/
template<class _type>
inline void
ReleaseGlobalResource() ythrow()
{
	return GStaticCache<_type, GHandle<_type> >::Release();
}

//应用程序资源类型定义。

YSL_BEGIN_NAMESPACE(Drawing)

//图像资源。
class YImage : public GMCounter<YImage>, public YCountableObject,
	public BitmapBuffer,
	implements IResource
{
public:
	typedef YCountableObject ParentType;

	/*!
	\brief 构造：使用指定位图指针、长和宽。
	*/
	explicit
	YImage(ConstBitmapPtr = NULL, SDst = 0, SDst = 0);

	DefGetter(BitmapPtr, ImagePtr, GetBufferPtr())

	void
	SetImage(ConstBitmapPtr, SDst = Global::MainScreenWidth,
		SDst = Global::MainScreenHeight);
};

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif


﻿/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Host.h
\ingroup YCLib
\ingroup Host
\brief YCLib 宿主平台公共扩展。
\version r93
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2014-10-13 21:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Host
*/


#ifndef YCL_INC_Host_h_
#define YCL_INC_Host_h_ 1

#include "YCLib/YModules.h"
#include "YSLib/Core/YModules.h"
#include YFM_YCLib_YCommon
#include YFM_YCLib_NativeAPI
#include YFM_YSLib_Core_YException // for YSLib::LoggedEvent;
#include <memory> // for std::unique_ptr;
#include <system_error> // for std::system_error;

#if YF_Hosted

namespace platform_ex
{

/*!
\ingroup exception_types
\brief 宿主异常。
\since build 426
*/
class YF_API Exception : public std::system_error
{
public:
	//! \since build 545
	//@{
	using LevelType = YSLib::LoggedEvent::LevelType;

private:
	LevelType level = YSLib::Emergent;

public:
	Exception(std::error_code, const std::string& = "unknown host exception",
		LevelType = YSLib::Emergent);
	Exception(int, const std::error_category&, const std::string&
		= "unknown host exception", LevelType = YSLib::Emergent);

	DefGetter(const ynothrow, LevelType, Level, level)
	//@}
};


/*!
\brief 句柄删除器。
\since build 520
*/
struct YF_API HandleDeleter
{
#	if YCL_Win32
	using pointer = ::HANDLE;

	PDefHOp(void, (), pointer h)
		ImplExpr(::CloseHandle(h))
#else
	using pointer = int*;

	PDefHOp(void, (), pointer h)
		ImplExpr(delete h)
#	endif
};

//! \since build 520
using UniqueHandle = std::unique_ptr<HandleDeleter::pointer, HandleDeleter>;

} // namespace platform_ex;

#endif

#endif


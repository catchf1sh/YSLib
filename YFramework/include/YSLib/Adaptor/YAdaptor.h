﻿/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YAdaptor.h
\ingroup Adaptor
\brief 外部库关联。
\version r1773
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-02-22 20:16:21 +0800
\par 修改时间:
	2015-06-01 16:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YAdaptor
*/


#ifndef YSL_INC_Adaptor_YAdaptor_h_
#define YSL_INC_Adaptor_YAdaptor_h_ 1

#include "YModules.h"

#include YFM_YSLib_Adaptor_YNew
#include <libdefect/cmath.h>
#include <ystdex/algorithm.hpp> // for ystdex::min, ystdex::max;
#include <ystdex/functional.hpp>
#include <ystdex/utility.hpp>
#include <ystdex/tuple.hpp>
#include YFM_YCLib_YCommon
#include YFM_YCLib_Debug
#include YFM_YCLib_Keys
#include YFM_YCLib_Timer
#include YFM_YCLib_FileSystem
#include YFM_YCLib_Video
#include YFM_YCLib_Container
#include YFM_YCLib_Mutex
#include YFM_YCLib_Reference

/*
!\brief YSLib 命名空间。
\since 早于 build 132
*/
namespace YSLib
{

//! \since build 593
//@{
using platform::byte;
using platform::octet;
using platform::ptrdiff_t;
using platform::size_t;
using platform::wint_t;
//@}


/*!
\brief 调用分派。
\since build 303
*/
//@{
using ystdex::seq_apply;
using ystdex::unseq_apply;
//@}

/*!
\brief 实用类型。
\since build 209
*/
//@{
using ystdex::noncopyable;
//! \since build 373
using ystdex::nonmovable;
using ystdex::nullptr_t;
//@}

/*!
\brief 数学库函数。
\since build 301
*/
//@{
using std::round;
//@}

/*!
\brief 算法。
\since build 265
*/
//@{
#if __cplusplus >= 201402L
using std::min;
using std::max;
#else
//! \since build 578
using ystdex::min;
//! \since build 578
using ystdex::max;
#endif
//}


//! \since build 593
//@{
using platform::forward_as_tuple;
using platform::get;
using platform::ignore;
using platform::make_pair;
using platform::make_tuple;
using platform::pair;
using platform::tie;
using platform::tuple;
using platform::tuple_cat;

//! \since build 546
using platform::begin;
//! \since build 546
using platform::end;

using platform::array;
using platform::deque;
using platform::forward_list;
using platform::list;
using platform::vector;

using platform::map;
using platform::multimap;
using platform::multiset;
using platform::set;

using platform::unordered_map;
using platform::unordered_multimap;
using platform::unordered_multiset;
using platform::unordered_set;

using platform::stack;
using platform::priority_queue;
using platform::queue;

using platform::arrlen;

using platform::bad_weak_ptr;
using platform::const_pointer_cast;
using platform::dynamic_pointer_cast;
using platform::enable_shared_from_this;
using platform::get_deleter;
using platform::make_shared;
using platform::make_unique;
//! \since build 601
using platform::make_unique_default_init;
using platform::get_raw;
using platform::owner_less;
using platform::reset;
using platform::share_raw;
using platform::shared_ptr;
using platform::static_pointer_cast;
using platform::unique_raw;
using platform::unique_ptr;
using platform::weak_ptr;

using platform::lref;
//@}

/*!
\brief 解锁删除器：使用线程模型对应的互斥量和锁。
\since build 590
*/
using platform::Threading::unlock_delete;


/*!
\brief 并发操作。
\since build 551
*/
using namespace platform::Concurrency;
/*!
\brief 平台公用描述。
\since build 456
*/
using namespace platform::Descriptions;

/*!
\brief 基本实用例程。
\since build 177
*/
//@{
//! \since build 547
using platform::usystem;
//@}

/*!
\brief 日志。
\since build 510
*/
//@{
using platform::Logger;
using platform::FetchCommonLogger;
//@}

/*!
\brief 断言操作。
\pre 允许对操作数使用 ADL 查找同名声明，但要求最终的表达式语义和调用这里的声明等价。
\since build 553
*/
//@{
using platform::Nonnull;
//! \since build 566
using platform::CheckIter;
using platform::Deref;
//@}

/*!
\brief 文件系统例程。
\since build 299
*/
//@{
//! \since build 549
using platform::uaccess;
//! \since build 549
using platform::uopen;
using platform::ufopen;
using platform::ufexists;
//! \since build 566
using platform::upclose;
//! \since build 566
using platform::upopen;
//! \since build 304
using platform::u16getcwd_n;
//! \since build 313
using platform::uchdir;
//! \since build 475
using platform::umkdir;
//! \since build 475
using platform::urmdir;
//! \since build 476
using platform::uunlink;
//! \since build 476
using platform::uremove;
//! \since build 341
using platform::truncate;
//! \since build 547
using platform::GetFileModificationTimeOf;
//! \since build 547
using platform::GetFileSizeOf;
//@}

//系统处理函数。
using platform::terminate;

//基本图形定义。
using platform::SPos;
using platform::SDst;

//基本输出接口。
using platform::InitVideo;

//计时器和时钟。
using platform::GetTicks;
using platform::GetHighResolutionTicks;

//输入类型。
//! \since build 490
using platform::KeyIndex;
//! \since build 486
using platform::KeyBitsetWidth;
using platform::KeyInput;
//! \since build 489
namespace KeyCategory = platform::KeyCategory;
namespace KeyCodes = platform::KeyCodes;

//! \since build 486
using platform::FindFirstKey;
//! \since build 487
using platform::FindNextKey;
//! \since build 487
using platform::MapKeyChar;

//! \brief 图形处理。
namespace Drawing
{

//! \since build 441
using platform::BGRA;
//! \since build 441
using platform::RGBA;
//! \since build 559
using platform::Pixel;
/*!
\since build 297
*/
//@{
using platform::FetchAlpha;
using platform::FetchOpaque;
//@}
//! \since build 417
//@{
using platform::MonoType;
using platform::AlphaType;
//@}
using platform::Color;
namespace ColorSpace = platform::ColorSpace;

} // namespace Drawing;

namespace IO
{

//! \since build 402
using platform::CS_Path;

//! \since build 474
using platform::NodeCategory;

//! \since build 411
//@{
using platform::FileOperationFailure;
using platform::DirectorySession;
using platform::HDirectory;
using platform::FileIterator;
//@}
using platform::IsAbsolute;
using platform::GetRootNameLength;

} // namespace IO;

} // namespace YSLib;

#endif


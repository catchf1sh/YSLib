﻿/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.h
\ingroup Service
\brief 平台中立的文件系统抽象。
\version r2497
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2010-03-28 00:09:28 +0800
\par 修改时间:
	2015-05-29 19:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::FileSystem
*/


#ifndef YSL_INC_Service_FileSystem_h_
#define YSL_INC_Service_FileSystem_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YString
#include <ystdex/path.hpp>

namespace YSLib
{

namespace IO
{

/*!
\brief 目录路径结尾正规化。
\since build 540
*/
template<class _tString>
inline ystdex::decay_t<_tString>
NormalizeDirectoryPathTail(_tString&& str, typename
	ystdex::string_traits<_tString>::const_pointer tail = &ystdex::to_array<
	typename ystdex::string_traits<_tString>::value_type>("/\\")[0])
{
	return ystdex::rtrim(yforward(str), tail) + typename
		ystdex::string_traits<_tString>::value_type(YCL_PATH_DELIMITER);
}


/*!
\brief 文件路径范式。
\since build 473
*/
class YF_API PathNorm : public ystdex::path_norm<YSLib::String>
{
public:
	using value_type = YSLib::String;

	//! \since build 586
	//@{
	DefDeCtor(PathNorm)
	DefDeCopyCtor(PathNorm)
	//! \brief 虚析构：类定义外默认实现。
	~PathNorm() override;
	//@}

	PDefH(bool, is_delimiter, const value_type& str) override
		ImplRet(YCL_FS_CharIsDelimiter(str[0], u))

	PDefH(bool, is_parent, const value_type& str) ynothrow override
		ImplRet(YCL_FS_StringIsParent(str, u))

	PDefH(bool, is_root, const value_type& str) ynothrow override
		ImplRet(YCL_FS_StringIsRoot(str, u))

	PDefH(bool, is_self, const value_type& str) ynothrow override
		ImplRet(YCL_FS_StringIsCurrent(str, u))

	//! \since build 475
	DefClone(const override, PathNorm)
};


//! \since build 409
using ypath = ystdex::path<vector<String>, PathNorm>;


/*!
\brief 路径。
\warning 非虚析构。
*/
class YF_API Path : private ypath, public ystdex::totally_ordered<Path>,
	public ystdex::dividable<Path, String>, public ystdex::dividable<Path>
{
public:
	using ypath::iterator;
	using ypath::const_iterator;
	//! \since build 409
	using ypath::value_type;

public:
	/*!
	\brief 无参数构造：默认实现。
	*/
	DefDeCtor(Path)
	explicit
	Path(const ucs2_t* str)
		: ypath(Parse(str))
	{}
	//! \since build 402
	//@{
	explicit
	Path(const ucs2string& str)
		: ypath(Parse(str))
	{}
	explicit
	Path(ucs2string&& str)
		: ypath(Parse(str))
	{}
	//! \since build 448
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_ctor_t<Path, _type>)>
	explicit
	Path(_type&& arg, Text::Encoding enc = CS_Path)
		: ypath(Parse(String(yforward(arg), enc)))
	{}
	//! \since build 599
	template<typename _tIn>
	Path(_tIn first, _tIn last)
		: ypath(first, last)
	{}
	//! \since build 599
	template<typename _type>
	Path(std::initializer_list<_type> il)
		: ypath(il)
	{}
	//@}
	/*!
	\brief 复制构造：默认实现。
	*/
	DefDeCopyCtor(Path)
	/*!
	\brief 转移构造：默认实现。
	*/
	DefDeMoveCtor(Path)
	DefDeDtor(Path)

	/*!
	\brief 复制赋值：默认实现。
	\since build 295
	*/
	DefDeCopyAssignment(Path)
	/*!
	\brief 转移赋值：默认实现。
	\since build 295
	*/
	DefDeMoveAssignment(Path)

	//! \brief 追加路径。
	//@{
	//! \since build 410
	PDefHOp(Path&, /=, const String& fname)
		ImplRet(GetBaseRef() /= fname, *this)
	PDefHOp(Path&, /=, const Path& pth)
		ImplRet(GetBaseRef() /= pth, *this)
	//@}

	friend PDefHOp(bool, ==, const Path& x, const Path& y)
		ImplRet(x.GetBase() == y.GetBase())

	friend PDefHOp(bool, <, const Path& x, const Path& y)
		ImplRet(x.GetBase() < y.GetBase())

	/*!
	\brief 转换为字符串。
	\sa Verify
	\since build 409
	*/
	DefCvt(const, String, Verify())
	/*!
	\brief 转换为窄字符串。
	\since build 411
	*/
	DefCvt(const, string, GetMBCS())

	//! \since build 600
	DefGetter(const ynothrow, const ypath&, Base, *this)
	//! \since build 600
	DefGetter(ynothrow, ypath&, BaseRef, *this)
	/*!
	\brief 取指定编码的多字节字符串。
	\since build 599
	*/
	PDefH(string, GetMBCS, Text::Encoding enc = CS_Path,
		ucs2_t delimiter = ucs2_t(YCL_PATH_DELIMITER)) const
		ImplRet(Verify(delimiter).GetMBCS(enc))
	/*!
	\brief 取指定分隔符的字符串表示。
	\post 断言：结果为空或以分隔符结尾。
	\since build 599
	*/
	String
	GetString(ucs2_t = ucs2_t(YCL_PATH_DELIMITER)) const;

	/*!
	\brief 正规化：去除自指和父节点的路径成员。
	\since build 410
	*/
	PDefH(void, Normalize, )
		ImplExpr(ystdex::normalize(*this))

	//! \since build 409
	//@{
	static ypath
	Parse(const ucs2string&);

	/*!
	\brief 转换为指定分隔符表示的字符串并验证。
	\note 使用 VerifyDirectory 验证，当且仅当确认为可打开的目录时结果以分隔符结尾。
	\sa GetString
	\sa VerifyDirectory
	\since build 599
	*/
	String
	Verify(ucs2_t = ucs2_t(YCL_PATH_DELIMITER)) const;

	using ypath::back;

	using ypath::begin;

	using ypath::cbegin;

	using ypath::clear;

	using ypath::cend;

	using ypath::empty;

	using ypath::end;

	using ypath::erase;

	//! \since build 410
	using ypath::filter_self;

	using ypath::front;

	//! \since build 410
	using ypath::get_norm;

	using ypath::insert;

	using ypath::is_absolute;

	using ypath::is_relative;

	//! \since build 599
	using ypath::max_size;

	//! \since build 410
	using ypath::merge_parents;

	//! \since build 473
	using ypath::pop_back;

	//! \since build 473
	using ypath::push_back;

	//! \since build 599
	using ypath::size;

	using ypath::swap;
	PDefH(void, swap, Path& pth)
		ImplExpr(pth.GetBaseRef().swap(*this))

	//! \since build 475
	friend PDefH(String, to_string, const Path& pth)
		ImplRet(to_string(pth.GetBase(), {ucs2_t(YCL_PATH_DELIMITER)}))
	//@}
};

/*!
\brief 交换。
\relates Path
*/
inline DefSwap(ynothrow, Path)


/*!
\brief 取扩展名。
\note 非贪婪匹配。
\since build 410
*/
//@{
//! \since build 599
template<class _tString>
_tString
GetExtensionOf(const _tString& fname)
{
	return ystdex::find_suffix(fname, typename _tString::value_type('.'));
}
inline PDefH(String, GetExtensionOf, const Path& pth)
	ImplRet(pth.empty() ? String() : GetExtensionOf(pth.back()))
//@}


/*!
\brief 默认目录路径长度。
\note 不小于 \c PATH_MAX 。
\since build 542
*/
yconstexpr const size_t MaxPathLength(yimpl(1 << 10));

/*!
\brief 取当前工作目录。
\post 结果长度不大于参数。
\note 不含结尾分隔符。
\since build 475
*/
YF_API String
FetchCurrentWorkingDirectory(size_t = MaxPathLength);

//! \since build 410
//@{
//! \brief 判断路径表示绝对路径。
//@{
inline PDefH(bool, IsAbsolute, const string& path)
	ImplRet(IsAbsolute(path.c_str()))
inline PDefH(bool, IsAbsolute, const String& path)
	ImplRet(IsAbsolute(path.GetMBCS(CS_Path)))
inline PDefH(bool, IsAbsolute, const Path& pth)
	ImplRet(!pth.empty() && IsAbsolute(pth.GetString()))
//@}

//! \brief 判断路径表示相对路径（包括空路径）。
//@{
inline PDefH(bool, IsRelative, const char* path)
	ImplRet(!IsAbsolute(path))
inline PDefH(bool, IsRelative, const string& path)
	ImplRet(!IsAbsolute(path))
inline PDefH(bool, IsRelative, const String& path)
	ImplRet(!IsAbsolute(path))
inline PDefH(bool, IsRelative, const Path& pth)
	ImplRet(!IsAbsolute(pth))
//@}

/*!
\brief 根据当前工作目录和指定路径取绝对路径。
\post 断言：结果是绝对路径。
\return 若指定路径为相对路径则为正规化当前工作目录追加此路径，否则为正规化指定路径。
\sa IO::IsRelative
\sa Path::Normalize
\since build 542
*/
YF_API Path
MakeNormalizedAbsolute(const Path&, size_t = MaxPathLength);


/*!
\brief 验证路径表示的目录是否存在且可打开。
\note 受权限限制。
*/
//@{
YF_API bool
VerifyDirectory(const char*);
inline PDefH(bool, VerifyDirectory, const string& path)
	ImplRet(VerifyDirectory(path.c_str()))
inline PDefH(bool, VerifyDirectory, const String& path)
	ImplRet(VerifyDirectory(path.GetMBCS(CS_Path)))
inline PDefH(bool, VerifyDirectory, const Path& pth)
	ImplRet(!pth.empty() && VerifyDirectory(pth.GetString()))
//@}


/*!
\brief 验证路径表示的目录是否存在，若不存在则逐级创建。
\throw std::system_error 失败时根据 \c errno 抛出的异常。
\note 使用 umkdir 实现。
\since build 563
*/
//@{
YF_API void
EnsureDirectory(const Path&);
inline PDefH(void, EnsureDirectory, const char* path)
	ImplExpr(EnsureDirectory(Path(path)))
inline PDefH(void, EnsureDirectory, const string& path)
	ImplExpr(EnsureDirectory(path.c_str()))
inline PDefH(void, EnsureDirectory, const String& path)
	ImplExpr(EnsureDirectory(path.GetMBCS(CS_Path)))
//@}


//! \since build 538
//@{
//! \brief 目录遍历操作。
//@{
template<typename _func>
void
Traverse(HDirectory& dir, _func f)
{
	PathNorm nm;

	std::for_each(FileIterator(&dir), FileIterator(), [&, f](const string& name)
		ynoexcept_spec(f(dir.GetNodeCategory(), name, nm)){
		YAssert(!name.empty(), "Empty name found.");
		if(!nm.is_self(name))
			f(dir.GetNodeCategory(), name, nm);
	});
}
template<typename _func>
inline void
Traverse(const char* path, _func f)
{
	HDirectory dir(path);

	IO::Traverse(dir, f);
}
//! \since build 593
template<typename _func>
inline void
Traverse(const string& path, _func f)
{
	IO::Traverse(path.c_str(), f);
}
template<typename _func>
inline void
Traverse(const Path& pth, _func f)
{
	IO::Traverse(string(pth), f);
}

//! \since build 593
template<typename _func>
void
TraverseChildren(const string& path, _func f)
{
	IO::Traverse(path, [f](NodeCategory c, const string& name, PathNorm& nm)
		ynoexcept_spec(f(c, name)){
		if(!nm.is_parent(name))
			f(c, name);
	});
}
//@}

//! \brief 删除参数指定路径的目录树。
//@{
YF_API void
DeleteTree(const Path&);
//! \since build 593
inline PDefH(void, DeleteTree, const string& pth)
	ImplRet(IO::DeleteTree(Path(pth)))
//@}

//! \brief 遍历目录中的项目，更新至列表。
YF_API void
ListFiles(const Path&, vector<String>&);
//@}


//! \brief 按文件系统节点类别对路径分类。
YF_API NodeCategory
ClassifyNode(const Path&);
//@}


// \brief 文件名过滤器。
// TODO: Definition & impl.

} // namespace IO;

} // namespace YSLib;

#endif


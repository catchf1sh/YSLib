﻿/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstring.h
\ingroup YStandardEx
\brief ISO C 标准字符串扩展。
\version r1889
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2009-12-27 17:31:14 +0800
\par 修改时间:
	2015-06-08 17:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CString
*/


#ifndef YB_INC_ystdex_cstring_h_
#define YB_INC_ystdex_cstring_h_ 1

#include "type_op.hpp" // for or_, is_same;
#include <cstring> // for std::strlen, std::strcpy, std::memchr, std::strncpy;
#include <string> // for std::char_traits;
#include "cassert.h" // for yconstraint;
#include "cctype.h" // for ystdex::tolower;
#include <cwchar> // for std::wmemchr, std::wcscpy, std::wcsncpy;

namespace ystdex
{

//非 ISO C/C++ 扩展库函数。
//

/*!
\brief 带空指针检查的字符串长度计算。
\return 当字符指针非空时为 std::strlen 计算的串长，否则为 0 。
*/
inline YB_PURE size_t
strlen_n(const char* s)
{
	return s ? std::strlen(s) : 0;
}

/*!
\brief 带空指针检查的字符串复制。
\return 成功时为复制的字符串，失败时为空指针。

当目标字符串和源字符串都非空时用 std::strcpy 复制字符串。
*/
inline char*
strcpy_n(char* d, const char* s)
{
	return d && s ? std::strcpy(d, s) : nullptr;
}

/*!
\brief 带空指针检查的字符串连接。
\return 返回目标参数。

对传入参数进行非空检查后串接指定的两个字符串，结果复制至指定位置。
*/
YB_API char*
strcpycat(char*, const char*, const char*);

/*!
\brief 带空指针检查的字符串连接复制。
\return 目标参数。

对传入参数进行非空检查后串接指定的两个字符串，
结果复制至用指定分配函数（默认为 std::malloc）新分配的空间。
*/
YB_API char*
strcatdup(const char*, const char*, void*(*)(size_t) = std::malloc);


/*!
\ingroup unary_type_traits
\brief 判断字符类型是否被 ISO C++ 指定提供 <tt>std::char_traits</tt> 的特化。
\since build 544
*/
template<typename _tChar>
struct is_char_specialized_in_std : or_<is_same<_tChar, char>,
	is_same<_tChar, wchar_t>, is_same<_tChar, char16_t>,
	is_same<_tChar, char32_t>>
{};


/*!
\brief 使用 <tt>std::char_traits::eq</tt> 判断是否为空字符。
\since build 329
*/
template<typename _tChar>
yconstfn YB_STATELESS bool
is_null(_tChar c)
{
	return std::char_traits<_tChar>::eq(c, _tChar());
}


//! \since build 544
namespace details
{

template<typename _tChar>
inline YB_PURE size_t
ntctslen_raw(const _tChar* s, std::true_type)
{
	return std::char_traits<_tChar>::length(s);
}
template<typename _tChar>
YB_PURE size_t
ntctslen_raw(const _tChar* s, std::false_type)
{
	const _tChar* p(s);

	while(!ystdex::is_null(*p))
		++p;
	return size_t(p - s);
}

} // namespace details;


/*!	\defgroup NTCTSUtil null-terminated character string utilities
\brief 简单 NTCTS 操作。
\note NTCTS(null-terminated character string) 即空字符标记结束的字符串，
	除了结束字符外没有空字符。
\note 简单指不包括 NTMBS(null-terminated mutibyte string) ，按等宽字符考虑。
\see ISO C++03 (17.1.12, 17.3.2.1.3.2) 。
*/
//@{
/*!
\brief 计算简单 NTCTS 长度。
\pre 断言： <tt>s</tt> 。
\note 语义同 std::char_traits<_tChar>::length 。
\since build 329
*/
template<typename _tChar>
inline YB_PURE size_t
ntctslen(const _tChar* s)
{
	yconstraint(s);

	return details::ntctslen_raw(s,
		typename is_char_specialized_in_std<_tChar>::type());
}

/*!
\brief 计算不超过指定长度的简单 NTCTS 长度。
\pre 断言： <tt>s</tt> 。
\note 语义同 std::char_traits<_tChar>::length ，但不超过指定值。
\since build 604
*/
//@{
template<typename _tChar>
YB_PURE size_t
ntctsnlen(const _tChar* s, size_t n)
{
	yconstraint(s);

	const auto str(s);

	while(n-- != 0 && *s)
		++s;

	return s - str;
}
inline YB_PURE size_t
ntctsnlen(const char* s, size_t n)
{
	yconstraint(s);

	const auto p(static_cast<const char*>(std::memchr(s, char(), n)));

	return p ? size_t(p - s) : n;
}
inline YB_PURE size_t
ntctsnlen(const wchar_t* s, size_t n)
{
	yconstraint(s);

	const auto p(static_cast<const wchar_t*>(std::wmemchr(s, char(), n)));

	return p ? size_t(p - s) : n;
}
//@}

/*!
\brief 按字典序比较简单 NTCTS 。
\pre 断言： <tt>s1 && s2</tt> 。
\note 语义同 std::basic_string<_tChar>::compare ，但忽略指定长度。
\since build 329
*/
template<typename _tChar>
YB_PURE typename std::char_traits<_tChar>::int_type
ntctscmp(const _tChar* s1, const _tChar* s2)
{
	yconstraint(s1),
	yconstraint(s2);

	typename std::char_traits<_tChar>::int_type d(0);

	while(!ystdex::is_null(d = *s1 - *s2))
		yunseq(++s1, ++s2);
	return d;
}

/*!
\brief 按字典序比较简单 NTCTS （忽略大小写）。
\pre 断言： <tt>s1 && s2</tt> 。
\note 语义同 std::basic_string<_tChar>::compare ，但忽略指定长度和大小写，
	使用字符串结束符判断结束。
\since build 329
*/
template<typename _tChar>
YB_PURE typename std::char_traits<_tChar>::int_type
ntctsicmp(const _tChar* s1, const _tChar* s2)
{
	yconstraint(s1),
	yconstraint(s2);

	using int_type = typename std::char_traits<_tChar>::int_type;
	int_type d(0);

	while((d = int_type(ystdex::tolower(*s1)) - int_type(ystdex::tolower(*s2)))
		== int_type(0) && !ystdex::is_null(s2))
		yunseq(++s1, ++s2);
	return d;
}

/*!
\brief 按字典序比较不超过指定长度的简单 NTCTS （忽略大小写）。
\pre 断言： <tt>s1 && s2</tt> 。
\note 语义同 std::basic_string<_tChar>::compare ，但忽略大小写，
	使用字符串结束符判断结束。
\since build 604
*/
template<typename _tChar>
YB_PURE typename std::char_traits<_tChar>::int_type
ntctsnicmp(const _tChar* s1, const _tChar* s2, size_t n)
{
	yconstraint(s1),
	yconstraint(s2);

	using int_type = typename std::char_traits<_tChar>::int_type;
	int_type d(0);

	while(n-- != 0 && (d = int_type(ystdex::tolower(*s1))
		- int_type(ystdex::tolower(*s2))) == int_type(0)
		&& !ystdex::is_null(*s2))
		yunseq(++s1, ++s2);
	return d;
}

/*!
\pre 断言： <tt>s1 && s2</tt> 。
\pre 复制的 NTCTS 存储不重叠。
*/
//@{
/*!
\brief 复制 NTCTS 。
\since build 604
*/
//@{
template<typename _tChar>
_tChar*
ntctscpy(_tChar* s1, const _tChar* s2)
{
	yconstraint(s1),
	yconstraint(s2);

	const auto res(s1);

	while(!ystdex::is_null(*s1++ = *s2++))
		;
	return res;
}
inline char*
ntctscpy(char* s1, const char* s2)
{
	yconstraint(s1),
	yconstraint(s2);

	return std::strcpy(s1, s2);
}
inline wchar_t*
ntctscpy(wchar_t* s1, const wchar_t* s2)
{
	yconstraint(s1),
	yconstraint(s2);

	return std::wcscpy(s1, s2);
}
//@}
/*!
\brief 复制确定源长度的 NTCTS 。
\pre 断言： <tt>s1 && s2</tt> 。
\since build 329
*/
template<typename _tChar>
_tChar*
ntctscpy(_tChar* s1, const _tChar* s2, size_t n)
{
	yconstraint(s1),
	yconstraint(s2);

	return yunseq(std::char_traits<_tChar>::copy(s1, s2, n), s1[n] = _tChar());
}

/*!
\brief 复制不超过指定长度的 NTCTS 。
\note 目标字符串短于指定长度的部分会被填充空字符。
\warning 源字符串在指定长度内没有空字符则目标字符串不以空字符结尾。
\since build 604
*/
//@{
template<typename _tChar>
_tChar*
ntctsncpy(_tChar* s1, const _tChar* s2, size_t n)
{
	yconstraint(s1),
	yconstraint(s2);

	const auto res(s1);

	while(n != 0)
	{
		--n;
		if(ystdex::is_null(*s1++ = *s2++))
			break;
	}
	while(n-- != 0)
		*s2++ = _tChar();
	return res;
}
inline char*
ntctsncpy(char* s1, const char* s2, size_t n)
{
	yconstraint(s1),
	yconstraint(s2);

	return std::strncpy(s1, s2, n);
}
inline wchar_t*
ntctsncpy(wchar_t* s1, const wchar_t* s2, size_t n)
{
	yconstraint(s1),
	yconstraint(s2);

	return std::wcsncpy(s1, s2, n);
}
//@}
//@}


/*!
\ingroup constexpr_algorithms
\brief 计算简单 NTCTS 长度。
\note 语义同 std::char_traits<_tChar>::length 。
\since build 329
*/
template<typename _tChar>
yconstfn YB_PURE size_t
const_ntctslen(const _tChar* s)
{
	return ystdex::is_null(*s) ? 0 : ystdex::const_ntctslen(s + 1) + 1;
}

/*!
\ingroup constexpr_algorithms
\brief 计算简单 NTCTS 中的指定字符数。
\since build 329
*/
template<typename _tChar>
yconstfn YB_PURE size_t
const_ntctscnt(const _tChar* s, _tChar c)
{
	return ystdex::is_null(*s) ? 0 : ystdex::const_ntctscnt(s + 1, c)
		+ std::char_traits<_tChar>::eq(*s, c);
}

/*!
\ingroup constexpr_algorithms
\brief 比较简单 NTCTS 。
\note 语义同 std::basic_string<_tChar>::compare ，但忽略指定长度。
\since build 329
*/
template<typename _tChar>
yconstfn YB_PURE typename std::char_traits<_tChar>::int_type
const_ntctscmp(const _tChar* s1, const _tChar* s2)
{
	return !std::char_traits<_tChar>::eq(*s1, *s2) || ystdex::is_null(*s1)
		|| ystdex::is_null(*s2) ? *s1 - *s2
		: ystdex::const_ntctscmp(s1 + 1, s2 + 1);
}
/*!
\ingroup constexpr_algorithms
\brief 比较限制长度上限的简单 NTCTS 。
\note 语义同 std::basic_string<_tChar>::compare 。
\since build 329
*/
template<typename _tChar>
yconstfn YB_PURE typename std::char_traits<_tChar>::int_type
const_ntctscmp(const _tChar* s1, const _tChar* s2, size_t n)
{
	return n == 0 ? _tChar() : (!std::char_traits<_tChar>::eq(*s1, *s2)
		|| ystdex::is_null(*s1) || ystdex::is_null(*s2) ? *s1 - *s2
		: ystdex::const_ntctscmp(s1 + 1, s2 + 1, n - 1));
}

/*!
\ingroup constexpr_algorithms
\brief 在简单 NTCTS 中顺序查找指定字符。
\return 在查找结束时经过的字符数。
\since build 329
*/
template<typename _tChar>
yconstfn YB_PURE size_t
const_ntctschr(const _tChar* s, _tChar c)
{
	return ystdex::is_null(*s) || std::char_traits<_tChar>::eq(*s, c)
		? 0 : ystdex::const_ntctschr(s + 1, c) + 1;
}

/*!
\ingroup constexpr_algorithms
\brief 在简单 NTCTS 中顺序查找第指定次数出现的指定字符。
\return 在查找结束时经过的字符数。
\since build 329
*/
template<typename _tChar>
yconstfn YB_PURE size_t
const_ntctschrn(const _tChar* s, _tChar c, size_t n)
{
	return n == 0 || ystdex::is_null(*s) ? 0 : (std::char_traits<_tChar>
		::eq(*s, c) ? ystdex::const_ntctschrn(s + 1, c, n - 1) + (n != 1)
		: ystdex::const_ntctschrn(s + 1, c, n) + 1);
}

/*!
\ingroup constexpr_algorithms
\brief 在简单 NTCTS 中顺序查找作为子串的指定 NTCTS 。
\return 在查找结束时经过的字符数。
\since build 329
*/
template<typename _tChar>
yconstfn YB_PURE size_t
const_ntctsstr(const _tChar* s1, const _tChar* s2)
{
	return ystdex::is_null(*s1) ? 0 : (ystdex::const_ntctscmp(s1, s2,
		ystdex::const_ntctslen(s2)) == _tChar()? 0
		: ystdex::const_ntctsstr(s1 + 1, s2) + 1);
}
//@}

} // namespace ystdex;

#endif


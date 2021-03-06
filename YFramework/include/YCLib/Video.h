﻿/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Video.h
\ingroup YCLib
\brief 平台相关的视频输出接口。
\version r1246
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2011-05-26 19:41:08 +0800
\par 修改时间:
	2015-05-29 19:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Video
*/


#ifndef YCL_INC_Video_h_
#define YCL_INC_Video_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#include <ystdex/cstdint.hpp>

namespace platform
{

#if YCL_Win32
using SPos = long;
using SDst = unsigned long;
#else
using SPos = std::int16_t; //!< 屏幕坐标度量。
using SDst = std::uint16_t; //!< 屏幕坐标距离。
#endif

//! \since build 517
//@{
static_assert(std::is_signed<SPos>(),
	"Only signed integer is supported for screen position measurement.");
static_assert(std::is_unsigned<SDst>(),
	"Only unsigned integer is supported for screen destination measurement.");
static_assert(ystdex::integer_width<SDst>() >= ystdex::integer_width<SPos>(),
	"Max value of screen destination shall be grater than max value of screen"
	" position.");
//@}

/*!
\brief XYZA 特征。
\note X 、 Y 和 Z 表示色彩分量， A 表示 Alpha 通道分量。
\note 也适用于 AXYZ 。
\since build 441
*/
template<size_t _vX, size_t _vY, size_t _vZ, size_t _vA>
struct XYZATrait
{
	static yconstexpr const size_t ABitsN = _vA;
	static yconstexpr const size_t XBitsN = _vX;
	static yconstexpr const size_t YBitsN = _vY;
	static yconstexpr const size_t ZBitsN = _vZ;
	static yconstexpr const size_t XYBitsN = XBitsN + YBitsN;
	static yconstexpr const size_t XYZBitsN = XBitsN + YBitsN + ZBitsN;
	static yconstexpr const size_t BitsN = XBitsN + YBitsN + ZBitsN + ABitsN;
	static yconstexpr const size_t BytesN = (BitsN + CHAR_BIT - 1) / CHAR_BIT;

	using AType = typename ystdex::make_width_int<ABitsN>::unsigned_least_type;
	using BType = typename ystdex::make_width_int<XBitsN>::unsigned_least_type;
	using GType = typename ystdex::make_width_int<YBitsN>::unsigned_least_type;
	using RType = typename ystdex::make_width_int<ZBitsN>::unsigned_least_type;
	using IntegerType
		= typename ystdex::make_width_int<BitsN>::unsigned_least_type;
	using ArrayType = byte[BytesN];

	static_assert(BitsN <= 64, "Width larger than 64 unimplemented");
};


/*!
\brief XYZA 掩码特征。
\since build 507
*/
template<size_t _vX, size_t _vY, size_t _vZ, size_t _vA>
struct XYZAMaskTrait
{
	using IntegerType = typename XYZATrait<_vX, _vY, _vZ, _vA>::IntegerType;

	static yconstexpr const IntegerType XMaskN = _vX;
	static yconstexpr const IntegerType XYMaskN = XMaskN + _vY;
	static yconstexpr const IntegerType XYZMaskN = XYMaskN + _vZ;
	static yconstexpr const IntegerType AMask = ((1U << _vA) - 1) << XYZMaskN;
	static yconstexpr const IntegerType XMask = (1U << _vX) - 1;
	static yconstexpr const IntegerType YMask = ((1U << _vY) - 1) << XMaskN;
	static yconstexpr const IntegerType ZMask = ((1U << _vZ) - 1) << XYMaskN;
};


/*!
\brief AXYZ 掩码特征。
\since build 507
*/
template<size_t _vA, size_t _vX, size_t _vY, size_t _vZ>
struct AXYZMaskTrait
{
	using IntegerType = typename XYZATrait<_vX, _vY, _vZ, _vA>::IntegerType;

	static yconstexpr const IntegerType AMaskN = _vA;
	static yconstexpr const IntegerType AXMaskN = AMaskN + _vX;
	static yconstexpr const IntegerType AXYMaskN = AXMaskN + _vY;
	static yconstexpr const IntegerType AMask = (1U << _vA) - 1;
	static yconstexpr const IntegerType XMask = ((1U << _vX) - 1) << AMaskN;
	static yconstexpr const IntegerType YMask = ((1U << _vY) - 1) << AXMaskN;
	static yconstexpr const IntegerType ZMask = ((1U << _vZ) - 1) << AXYMaskN;
};


/*!
\brief BGRA 四元组。
\note 作为 POD 类型，可以用于储存像素。
\warning 用户应检查存储表示的实际大小是否和本类型一致。
\bug 当前仅支持小端整数字节序。
\bug 当前忽略对齐和特定类型的一致性。
\since build 441
*/
template<size_t _vB, size_t _vG, size_t _vR, size_t _vA>
union YB_ATTR(packed) YB_ATTR(
	aligned(yalignof(typename XYZATrait<_vB, _vG, _vR, _vA>::IntegerType))) BGRA
{
	using Trait = XYZATrait<_vB, _vG, _vR, _vA>;
	//! \since build 507
	using MaskTrait = XYZAMaskTrait<_vB, _vG, _vR, _vA>;
	//! \since build 555
	using ArrayType = typename Trait::ArrayType;
	//! \since build 555
	using IntegerType = typename Trait::IntegerType;

	ArrayType Bytes;
	IntegerType Integer;

//#if !LITTLE_ENDIAN
//#	error Unsupported integer endianness found.
//#endif

	DefDeCtor(BGRA)
	yconstfn
	BGRA(typename Trait::IntegerType i)
		: Integer(i)
	{}
	yconstfn
	BGRA(typename Trait::BType b, typename Trait::GType g,
		typename Trait::RType r, typename Trait::AType a)
		: Integer(IntegerType(b) | IntegerType(g) << MaskTrait::XMaskN
		| IntegerType(r) << MaskTrait::XYMaskN
		| IntegerType(a) << MaskTrait::XYZMaskN)
	{}

	//! \since build 442
	yconstfn DefCvt(const ynothrow, typename Trait::IntegerType, Integer)

	//! \since build 448
	yconstfn DefGetter(const ynothrow, typename Trait::AType, A,
		(Integer & MaskTrait::AMask) >> MaskTrait::XYZMaskN)
	yconstfn DefGetter(const ynothrow, typename Trait::BType, B,
		Integer & MaskTrait::XMask)
	//! \since build 448
	yconstfn DefGetter(const ynothrow, typename Trait::GType, G,
		(Integer & MaskTrait::YMask) >> MaskTrait::XMaskN)
	//! \since build 448
	yconstfn DefGetter(const ynothrow, typename Trait::RType, R,
		(Integer & MaskTrait::ZMask) >> MaskTrait::XYMaskN)
};


/*!
\brief RGBA 四元组。
\note 作为 POD 类型，可以用于储存像素。
\warning 用户应检查存储表示的实际大小是否和本类型一致。
\bug 当前仅支持小端整数字节序。
\bug 当前忽略对齐和特定类型的一致性。
\since build 441
*/
template<size_t _vR, size_t _vG, size_t _vB, size_t _vA>
union YB_ATTR(packed) YB_ATTR(
	aligned(yalignof(typename XYZATrait<_vB, _vG, _vR, _vA>::IntegerType))) RGBA
{
	using Trait = XYZATrait<_vR, _vG, _vB, _vA>;
	//! \since build 507
	using MaskTrait = XYZAMaskTrait<_vB, _vG, _vR, _vA>;
	//! \since build 555
	using ArrayType = typename Trait::ArrayType;
	//! \since build 555
	using IntegerType = typename Trait::IntegerType;

	ArrayType Bytes;
	IntegerType Integer;

//#if !LITTLE_ENDIAN
//#	error Unsupported integer endianness found.
//#endif

	DefDeCtor(RGBA)
	yconstfn
	RGBA(typename Trait::IntegerType i)
		: Integer(i)
	{}
	yconstfn
	RGBA(typename Trait::BType r, typename Trait::GType g,
		typename Trait::RType b, typename Trait::AType a)
		: Integer(IntegerType(r) | IntegerType(g) << MaskTrait::XMaskN
		| IntegerType(b) << MaskTrait::XYMaskN
		| IntegerType(a) << MaskTrait::XYZMaskN)
	{}

	//! \since build 442
	yconstfn DefCvt(const ynothrow, typename Trait::IntegerType, Integer)

	//! \since build 448
	yconstfn DefGetter(const ynothrow, typename Trait::AType, A,
		(Integer & MaskTrait::AMask) >> MaskTrait::XYZMaskN)
	yconstfn DefGetter(const ynothrow, typename Trait::BType, B,
		(Integer & MaskTrait::ZMask) >> MaskTrait::XYMaskN)
	//! \since build 448
	yconstfn DefGetter(const ynothrow, typename Trait::GType, G,
		(Integer & MaskTrait::YMask) >> MaskTrait::XMaskN)
	//! \since build 448
	yconstfn DefGetter(const ynothrow, typename Trait::RType, R,
		Integer & MaskTrait::XMask)
};


/*!
\brief ARGB 四元组。
\note 作为 POD 类型，可以用于储存像素。
\warning 用户应检查存储表示的实际大小是否和本类型一致。
\bug 当前仅支持小端整数字节序。
\bug 当前忽略对齐和特定类型的一致性。
\since build 507
*/
template<size_t _vA, size_t _vR, size_t _vG, size_t _vB>
union YB_ATTR(packed) YB_ATTR(
	aligned(yalignof(typename XYZATrait<_vB, _vG, _vR, _vA>::IntegerType))) ARGB
{
	using Trait = XYZATrait<_vB, _vG, _vR, _vA>;
	using MaskTrait = AXYZMaskTrait<_vA, _vR, _vG, _vB>;

	typename Trait::ArrayType Bytes;
	typename Trait::IntegerType Integer;

//#if !LITTLE_ENDIAN
//#	error Unsupported integer endianness found.
//#endif

	DefDeCtor(ARGB)
	yconstfn
	ARGB(typename Trait::IntegerType i)
		: Integer(i)
	{}
	yconstfn
	ARGB(typename Trait::BType a, typename Trait::GType r,
		typename Trait::RType g, typename Trait::AType b)
		: Integer(a | r << MaskTrait::AMaskN | g << MaskTrait::AXMaskN
		| b << MaskTrait::AXYMaskN)
	{}

	yconstfn DefCvt(const ynothrow, typename Trait::IntegerType, Integer)

	yconstfn DefGetter(const ynothrow, typename Trait::AType, A,
		Integer & MaskTrait::AMask)
	yconstfn DefGetter(const ynothrow, typename Trait::BType, B,
		(Integer & MaskTrait::ZMask) >> MaskTrait::AXYMaskN)
	yconstfn DefGetter(const ynothrow, typename Trait::GType, G,
		(Integer & MaskTrait::YMask) >> MaskTrait::AXMaskN)
	yconstfn DefGetter(const ynothrow, typename Trait::RType, R,
		(Integer & MaskTrait::XMask) >> MaskTrait::AMaskN)
};


//! \since build 417
//@{
using MonoType = octet;
using AlphaType = octet;
//@}

#if YCL_DS
	/*!
	\brief 标识 XYZ1555 像素格式。
	\note 值表示按整数表示的顺序从高位到低位为 ABGR 。
	\since build 297
	*/
#	define YCL_PIXEL_FORMAT_XYZ555 0xAABBCCDD

/*!
\brief LibNDS 兼容像素。
\relates Pixel
\note 小端序整数表示 ABGR1555 。
*/
using Pixel = RGBA<5, 5, 5, 1>;
/*!
\brief 取像素 Alpha 值。
\relates Pixel
\since build 417
*/
yconstfn PDefH(AlphaType, FetchAlpha, Pixel px) ynothrow
	ImplRet(px.GetA() != 0 ? 0xFF : 0)

/*!
\brief 取不透明像素。
\relates Pixel
\since build 413
*/
yconstfn PDefH(Pixel, FetchOpaque, Pixel px) ynothrow
	ImplRet(px.Integer | 1 << 15)

/*!
\brief 使用 8 位 RGB 构造 std::uint16_t 类型像素。
\since build 441
*/
yconstfn PDefH(std::uint16_t, FetchPixel, MonoType r, MonoType g, MonoType b)
	ynothrow
	ImplRet(r >> 3 | std::uint16_t(g >> 3) << 5 | std::uint16_t(b >> 3) << 10)

#	define DefColorH_(hex, name) name = \
	(FetchPixel(((hex) >> 16) & 0xFF, ((hex) >> 8) & 0xFF, (hex) & 0xFF) \
	| 1 << 15)
#elif YCL_Win32 || YCL_Android || YCL_Linux || YCL_OS_X
// TODO: Real implementation for X11.
#	if YCL_Win32
/*!
\brief 标识 XYZ888 像素格式。
\note 值表示按整数表示的顺序从高位到低位为 ARGB 。
\since build 297
*/
#		define YCL_PIXEL_FORMAT_XYZ888 0xAADDCCBB

/*!
\brief Windows DIB 格式兼容像素。
\note MSDN 注明此处第 4 字节保留为 0 ，但此处使用作为 8 位 Alpha 值使用。
	即小端序整数 ARGB8888 （存储序 BGRA8888 ）。
\note 转换 DIB 在设备上下文绘制时无需转换格式，比 ::COLORREF 更高效。
\warning 仅用于屏幕绘制，不保证无条件兼容所有 DIB 。
\since build 441
\todo 断言对齐，保证类型兼容。
*/
using Pixel = BGRA<8, 8, 8, 8>;
#	else
/*!
\brief 标识 XYZ888 像素格式。
\note 值表示按整数表示的顺序从高位到低位为 ABGR 。
\since build 506
*/
#		define YCL_PIXEL_FORMAT_XYZ888 0xAABBCCDD
/*!
\brief Android 像素。
\note Android 设备可选多种格式，在此使用 WINDOW_FORMAT_RGBA_8888 。
\since build 506
\todo 断言对齐，保证类型兼容。
*/
using Pixel = RGBA<8, 8, 8, 8>;
#	endif

/*!
\brief 取像素 Alpha 值。
\since build 417
*/
yconstfn PDefH(AlphaType, FetchAlpha, Pixel px) ynothrow
	ImplRet(px.GetA())

/*!
\brief 取不透明像素。
\relates Pixel
\since build 413
*/
yconstfn PDefH(Pixel, FetchOpaque, Pixel px) ynothrow
#	if YCL_Win32
	ImplRet({px.GetB(), px.GetG(), px.GetR(), 0xFF})
#	else
	ImplRet({px.GetB(), px.GetG(), px.GetR(), 0xFF})
#	endif

/*!
\brief 使用 8 位 RGB 构造 std::uint32_t 像素。
\relates Pixel
\since build 417
*/
yconstfn PDefH(std::uint32_t, FetchPixel,
	AlphaType r, AlphaType g, AlphaType b) ynothrow
	ImplRet(std::uint32_t(r) | std::uint32_t(g) << 8 | std::uint32_t(b) << 16)

//! \since build 458 as workaround for Visual C++ 2013
#	if YB_HAS_CONSTEXPR
#		define YCL_FetchPixel(r, g, b) platform::FetchPixel(r, g, b)
#	else
#		define YCL_FetchPixel(r, g, b) \
	(std::uint32_t(r) | std::uint32_t(g) << 8 | std::uint32_t(b) << 16)
#	endif

/*!
\brief 定义 Windows DIB 格式兼容像素。
\note 得到的 32 位整数和 ::RGBQUAD 在布局上兼容。
\note Alpha 值为 0xFF 。
\relates Pixel
\since build 296
*/
#	define DefColorH_(hex, name) \
	name = (YCL_FetchPixel(((hex) >> 16) & 0xFF, \
		((hex) >> 8) & 0xFF, (hex) & 0xFF) << 8 | 0xFF)
#else
#	error "Unsupported platform found."
#endif


//! \brief 系统默认颜色空间。
namespace ColorSpace
{
//	#define DefColorA(r, g, b, name) name = ARGB16(1, r, g, b),
#define	HexAdd0x(hex) 0x##hex
#define DefColorH(hex_, name) DefColorH_(HexAdd0x(hex_), name)

/*!
\brief 默认颜色集。
\see http://www.w3schools.com/html/html_colornames.asp 。
\since build 416
*/
enum ColorSet : Pixel::Trait::IntegerType
{
	DefColorH(00FFFF, Aqua),
	DefColorH(000000, Black),
	DefColorH(0000FF, Blue),
	DefColorH(FF00FF, Fuchsia),
	DefColorH(808080, Gray),
	DefColorH(008000, Green),
	DefColorH(00FF00, Lime),
	DefColorH(800000, Maroon),
	DefColorH(000080, Navy),
	DefColorH(808000, Olive),
	DefColorH(800080, Purple),
	DefColorH(FF0000, Red),
	DefColorH(C0C0C0, Silver),
	DefColorH(008080, Teal),
	DefColorH(FFFFFF, White),
	DefColorH(FFFF00, Yellow)
};

#undef YCL_FetchPixel
#undef DefColorH
#undef DefColorH_
#undef HexAdd0x
} // namespace ColorSpace;


//! \brief 颜色。
class YF_API Color
{
public:
	using ColorSet = ColorSpace::ColorSet;

private:
	/*!
	\brief RGB 分量。
	\since build 276
	*/
	MonoType r = 0, g = 0, b = 0;
	/*!
	\brief Alpha 分量。
	\since build 276
	*/
	AlphaType a = 0;

public:
	/*!
	\brief 无参数构造：所有分量为 0 的默认颜色。
	\since build 319
	*/
	yconstfn
	Color() = default;
	/*!
	\brief 构造：使用本机颜色对象。
	\since build 319
	*/
	yconstfn
	Color(Pixel px) ynothrow
#if YCL_DS
		: r(px.GetR() << 3), g(px.GetG() << 3), b(px.GetB() << 3),
		a(FetchAlpha(px) ? 0xFF : 0x00)
#elif YCL_Win32 || YCL_Linux || YCL_OS_X
		: r(px.GetR()), g(px.GetG()), b(px.GetB()), a(px.GetA())
#endif
	{}
	/*!
	\brief 构造：使用默认颜色。
	\since build 319
	*/
	yconstfn
	Color(ColorSet cs) ynothrow
#if YCL_DS
		: Color(Pixel(cs))
#elif YCL_Win32 || YCL_Android || YCL_Linux || YCL_OS_X
		: r((cs & 0xFF00) >> 8), g((cs & 0xFF0000) >> 16),
		b((cs & 0xFF000000) >> 24), a(0xFF)
#endif
	{}
	/*!
	\brief 构造：使用 RGB 值和 alpha 位。
	\since build 319
	*/
	yconstfn
	Color(MonoType r_, MonoType g_, MonoType b_, AlphaType a_ = 0xFF) ynothrow
		: r(r_), g(g_), b(b_), a(a_)
	{}
	/*!
	\brief 构造：使用相同类型转换为单色的 RGB 值和 alpha位。
	\note 避免列表初始化时 narrowing 转换。
	\since build 360
	*/
	template<typename _tScalar>
	yconstfn
	Color(_tScalar r_, _tScalar g_, _tScalar b_, AlphaType a_ = 0xFF) ynothrow
		: Color(MonoType(r_), MonoType(g_), MonoType(b_), a_)
	{}

	/*!
	\brief 转换：本机颜色对象。
	\since build 319
	*/
	yconstfn
	operator Pixel() const ynothrow
	{
#if YCL_DS
		return int(a != 0) << 15 | FetchPixel(r, g, b);
#elif YCL_Win32
		return {b, g, r, a};
#elif YCL_Android || YCL_Linux || YCL_OS_X
		return {r, g, b, a};
#endif
	}

	/*!
	\brief 取 alpha 分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, A, a)
	/*!
	\brief 取蓝色分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, B, b)
	/*!
	\brief 取绿色分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, G, g)
	/*!
	\brief 取红色分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, R, r)
};


/*!
\brief 初始化视频输出。
\warning 不保证线程安全性。
*/
YF_API bool
InitVideo();

} // namespace platform;

#endif


﻿// YSLib::Shell::YGDI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-14 18:29:46;
// UTime = 2010-7-26 6:31;
// Version = 0.2270;


#include "ygdi.h"
#include "../Core/yexcept.h"

YSL_BEGIN

using namespace Exceptions;

YSL_BEGIN_NAMESPACE(Drawing)

#define BLT_ALPHA_BITS	8
#define BLT_MAX_ALPHA	((1 << BLT_ALPHA_BITS) - 1)
#define BLT_ROUND		(1 << (BLT_ALPHA_BITS - 1))
#define BLT_THRESHOLD	8
#define BLT_THRESHOLD2	128

namespace
{
	inline SPOS
	blitMinX(SPOS sx, SPOS dx)
	{
		return sx + vmax<int>(0, -dx);
	}
	inline SPOS
	blitMinY(SPOS sy, SPOS dy)
	{
		return sy + vmax<int>(0, -dy);
	}
	inline SPOS
	blitMaxX(SPOS minX, SDST sw, SDST dw, SPOS sx, SPOS dx, SDST cw)
	{
		return sx + vmin<int>(vmin<int>(dw - dx, sw - sx), cw);
	}
	inline SPOS
	blitMaxY(SPOS minY, SDST sh, SDST dh, SPOS sy, SPOS dy, SDST ch)
	{
		return sy + vmin<int>(vmin<int>(dh - dy, sh - sy), ch);
	}
}


void
blitScale(const SPoint& sp, const SPoint& dp, const SSize& ss, const SSize& ds, const SSize& sc,
		  int& minX, int& minY, int& maxX, int& maxY)
{
	minX = blitMinX(sp.X, dp.X);
	minY = blitMinY(sp.Y, dp.Y);
	maxX = blitMaxX(minX, ss.Width, ds.Width, sp.X, dp.X, sc.Width);
	maxY = blitMaxY(minY, ss.Height, ds.Height, sp.Y, dp.Y, sc.Height);
}


static void
blitFor(int deltaX, int deltaY,
		BitmapPtr dc, ConstBitmapPtr sc,
		int dInc, int sInc)
{
	deltaX *= sizeof(PixelType);
	for(int y(0); y < deltaY; ++y)
	{
		memcpy(dc, sc, deltaX);
		sc += sInc;
		dc += dInc;
	}
}
void
blit(BitmapPtr dst, const SSize& ds,
	 ConstBitmapPtr src, const SSize& ss,
	 const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blitFor(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X), src + minY * ss.Width + minX,
		ds.Width, ss.Width);
}
void
blitH(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blitFor(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width + vmax<int>(0, dp.X), src + minY * ss.Width + minX,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
static void
blitForU(int deltaX, int deltaY,
		 BitmapPtr dc, ConstBitmapPtr sc,
		 int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
			*dc-- = *sc++;
		sc += sInc;
		dc -= dInc;
	}
}
void
blitV(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blitForU(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X) + maxX - minX - 1, src + minY * ss.Width + minX,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
void
blitU(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blitForU(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width + vmax<int>(0, dp.X), src + minY * ss.Width + minX,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}

/*
void blit(u8* dst, SDST dw, SDST dh,
const u8* src, SDST sw, SDST sh,
const SPoint& sp, const SPoint& dp, const SSize& sc)
{
const int minX = blitMinX(sx, dx),
minY = blitMinY(sy, dy),
maxX = blitMaxX(minX, sw, dw, sx, dx, cw),
maxY = blitMaxY(minY, sh, dh, sy, dy, ch);
const u8* sc = src + minY * ss.Width + minX;
u8* dc = dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X);

for(int y = minY; y < maxY; y++)
{
memcpy(dc, sc, maxX - minX);
sc += sw;
dc += dw;
}
}
*/

static void
blit2For(int deltaX, int deltaY,
		 BitmapPtr dc, ConstBitmapPtr sc,
		 int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			if(*sc & BITALPHA)
				*dc = *sc;
			++sc;
			++dc;
		}
		sc += sInc;
		dc += dInc;
	}
}
void
blit2(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blit2For(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X), src + minY * ss.Width + minX,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}
void
blit2H(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blit2For(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width + vmax<int>(0, dp.X), src + minY * ss.Width + minX,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
static void
blit2ForU(int deltaX, int deltaY,
		  BitmapPtr dc, ConstBitmapPtr sc,
		  int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			if(*sc & BITALPHA)
				*dc = *sc;
			++sc;
			--dc;
		}
		sc += sInc;
		dc -= dInc;
	}
}
void
blit2V(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blit2ForU(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X) + maxX - minX - 1, src + minY * ss.Width + minX,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
void
blit2U(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blit2ForU(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width + vmax<int>(0, dp.X), src + minY * ss.Width + minX,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}

static void
blit2For(int deltaX, int deltaY,
		 BitmapPtr dc, ConstBitmapPtr sc, const u8* sa,
		 int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			*dc = ((*sa++ & 0x80) ? *sc : 0) | BITALPHA;
			++sc;
			++dc;
		}
		sc += sInc;
		dc += dInc;
	}
}
void
blit2(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const u8* srcA, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blit2For(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X), src + srcOffset, srcA + srcOffset,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}
void
blit2H(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blit2For(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width + vmax<int>(0, dp.X), src + srcOffset, srcA + srcOffset,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
static void
blit2ForU(int deltaX, int deltaY,
		  BitmapPtr dc, ConstBitmapPtr sc, const u8* sa,
		  int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			*dc = ((*sa++ & 0x80) ? *sc : 0) | BITALPHA;
			++sc;
			--dc;
		}
		sc += sInc;
		dc -= dInc;
	}
}
void
blit2V(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blit2ForU(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X) + maxX - minX - 1, src + srcOffset, srcA + srcOffset,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
void
blit2U(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blit2ForU(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width + vmax<int>(0, dp.X), src + srcOffset, srcA + srcOffset,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}

static inline void
biltAlphaPoint(BitmapPtr dc, ConstBitmapPtr sc, const u8* sa)
{
	int a = *sa;

	if(a >= BLT_THRESHOLD)
	{
		int s = *sc, d = *dc;

		if(d & BITALPHA && a <= BLT_MAX_ALPHA - BLT_THRESHOLD)
		{
			int dbr = (d & 0x1F) | (d << 6 & 0x1F0000), dg = d & 0x3E0;

			dbr = (dbr + (((((s & 0x1F) | (s << 6 & 0x1F0000)) - dbr) * a + BLT_ROUND) >> BLT_ALPHA_BITS));
			dg  = (dg  + ((((s & 0x3E0) - dg ) * a + BLT_ROUND) >> BLT_ALPHA_BITS));
			*dc = (dbr & 0x1F) | (dg & 0x3E0) | (dbr >> 6 & 0x7C00) | BITALPHA;
		}
		else if(a >= BLT_THRESHOLD2)
			*dc = s | BITALPHA;
	}
}
static void
blitAlphaFor(int deltaX, int deltaY,
			 BitmapPtr dc, ConstBitmapPtr sc, const u8* sa,
			 int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			biltAlphaPoint(dc, sc, sa);
			++sa;
			++sc;
			++dc;
		}
		sc += sInc;
		sa += sInc;
		dc += dInc;
	}
}
void
blitAlpha(BitmapPtr dst, const SSize& ds,
		  ConstBitmapPtr src, const u8* srcA, const SSize& ss,
		  const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blitAlphaFor(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X), src + srcOffset, srcA + srcOffset,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}
void
blitAlphaH(BitmapPtr dst, const SSize& ds,
		   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
		   const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blitAlphaFor(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width + vmax<int>(0, dp.X), src + srcOffset, srcA + srcOffset,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
static void
blitAlphaForU(int deltaX, int deltaY,
			  BitmapPtr dc, ConstBitmapPtr sc, const u8* sa,
			  int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			biltAlphaPoint(dc, sc, sa);
			++sa;
			++sc;
			--dc;
		}
		sc += sInc;
		sa += sInc;
		dc -= dInc;
	}
}
void
blitAlphaV(BitmapPtr dst, const SSize& ds,
		   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
		   const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blitAlphaForU(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X) + maxX - minX - 1, src + srcOffset, srcA + srcOffset,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
void
blitAlphaU(BitmapPtr dst, const SSize& ds,
		   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
		   const SPoint& sp, const SPoint& dp, const SSize& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blitAlphaForU(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width + vmax<int>(0, dp.X) + maxX - minX - 1, src + srcOffset, srcA + srcOffset,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}


bool
DrawHLineSeg(MGIC& g, SPOS y, SPOS x1, SPOS x2, PixelType c)
{
	YAssert(g.IsValid(),
		"In function \"void\n"
		"DrawHLineSeg(MGIC& g, SPOS y, SPOS x1, SPOS x2, PixelType c)\": \n"
		"The graphic device context is invalid.");

	if(isInIntervalRegular<int>(y, g.GetHeight())
		&& !((x1 < 0 && x2 < 0) || (x1 >= g.GetWidth() && x2 >= g.GetWidth())))
	{
		restrictInIntervalRegular(x1, 0, g.GetWidth());
		restrictInIntervalRegular(x2, 0, g.GetWidth());
		restrictLessEqual(x1, x2);
		FillSeq<PixelType>(&g.GetBufferPtr()[y * g.GetWidth() + x1], x2 - x1 + 1, c);
		return true;
	}
	return false;
}

bool
DrawVLineSeg(MGIC& g, SPOS x, SPOS y1, SPOS y2, PixelType c)
{
	YAssert(g.IsValid(),
		"In function \"void\n"
		"DrawVLineSeg(MGIC& g, SPOS x, SPOS y1, SPOS y2, PixelType c)\": \n"
		"The graphic device context is invalid.");

	if(isInIntervalRegular<int>(x, g.GetWidth())
		&& !((y1 < 0 && y2 < 0) || (y1 >= g.GetHeight() && y2 >= g.GetHeight())))
	{
		restrictInIntervalRegular(y1, 0, g.GetHeight());
		restrictInIntervalRegular(y2, 0, g.GetHeight());
		restrictLessEqual(y1, y2);
		FillVLine<PixelType>(&g.GetBufferPtr()[y1 * g.GetWidth() + x], y2 - y1 + 1, g.GetWidth(), c);
		return true;
	}
	return false;
}

namespace
{
	//倾斜直线光栅化函数。
	bool
	DrawObliqueLine(MGIC& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, PixelType c)
	{
		YAssert(y1 != y2,
			"In function \"static void\n"
			"DrawObliqueLine(MGIC& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, PixelType c)\": \n"
			"Not drawing an oblique line: the line is horizontal.");
		YAssert(x1 != x2,
			"In function \"static void\n"
			"DrawObliqueLine(MGIC& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, PixelType c)\": \n"
			"Not drawing an oblique line: the line is vertical.");

		if(SRect(g.GetSize()).IsInBoundsRegular(x1, y1) && SRect(g.GetSize()).IsInBoundsRegular(x2, y2))
		{
			//一般 Bresenham 算法：实现自 http://cg.sjtu.edu.cn/lecture_site/chap2/mainframe212.htm 伪代码。
			//起点 (x1, y1) 和终点 (x2, y2) 不同。

			const s8 sx(sgn(x2 - x1)), sy(sgn(y2 - y1));
			SDST dx(abs(x2 - x1)), dy(abs(y2 - y1));
			bool f(dy > dx);

			if(f)
				std::swap(dx, dy);

			//初始化误差项以补偿非零截断。
			const SDST dx2(dx << 1), dy2(dy << 1);
			int e(dy2 - dx);

			//主循环。
			while(dx--)
			{
				PutPixel(g, x1, y1, c);
				if(e >= 0)
				{
					if(f)
						x1 += sx;
					else
						y1 += sy;
					e -= dx2;
				}
				if(f)
					y1 += sy;
				else
					x1 += sx;
				e += dy2;
			}
			return true;
		}
		return false;
	}
}

bool
DrawLineSeg(MGIC& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, PixelType c)
{
	if(y1 == y2)
		return DrawHLineSeg(g, y1, x1, x2, c);
	else if(x1 == x2)
		return DrawVLineSeg(g, x1, y1, y2, c);
	else
		return DrawObliqueLine(g, x1, y1, x2, y2, c);
}

bool
DrawRect(MGIC& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, PixelType c)
{
	bool b(DrawVLineSeg(g, x1, y1, y2, c));
	b |= DrawHLineSeg(g, y2, x1, x2, c);
	b |= DrawVLineSeg(g, x2, y2, y1, c);
	b |= DrawHLineSeg(g, y1, x2, x1, c);
	return b;
}


MPadding::MPadding(SDST l, SDST r, SDST t, SDST b)
: Left(l), Right(r), Top(t), Bottom(b)
{}
MPadding::MPadding(u64 m)
: Left(m >> 48), Right((m >> 32) & 0xFFFF), Top((m >> 16) & 0xFFFF), Bottom(m & 0xFFFF)
{}

u64
MPadding::GetAll() const
{
	u64 r = (Left << 16) | Right;

	r = (r << 32) | (Top << 16) | Bottom;
	return r;
}

void
MPadding::SetAll(SDST l, SDST r, SDST t, SDST b)
{
	Left = l;
	Right = r;
	Top = t;
	Bottom = b;
}

MPadding&
MPadding::operator+=(const MPadding& m)
{
	Left += m.Left;
	Right += m.Right;
	Top += m.Top;
	Bottom += m.Bottom;
	return *this;
}
MPadding
operator+(const MPadding& a, const MPadding& b)
{
	return MPadding(a.Left + b.Left, a.Right + b.Right, a.Top + b.Top, a.Bottom + b.Bottom);
}


MBitmapBuffer::MBitmapBuffer(ConstBitmapPtr i, SDST w, SDST h)
: SSize(w, h), img(NULL)
{
	SetSize(w, h);
	if(i)
		memcpy(img, i, sizeof(PixelType) * GetArea());
}

void
MBitmapBuffer::SetSize(SPOS w, SPOS h)
{
	if(w <= 0 || h <= 0)
	{
		delete[] img;
		img = NULL;
	}
	else if(!img || Width != w || Height != h)
	{
		if(!img || w > Width || h > Height)
		{
			if(img)
				delete[] img;
			try
			{
				img = new PixelType[w * h];
			}
			catch(std::bad_alloc&)
			{
				throw MLoggedEvent("Allocation failed @@ MBitmapBuffer::SetSize(SPOS, SPOS);", 1);
			}
		}
	}
	Width = w;
	Height = h;
	ClearImage();
}
void
MBitmapBuffer::SetSizeSwap()
{
	SDST t = Width;

	Width = Height;
	Height = t;
	ClearImage();
}

void
MBitmapBuffer::ClearImage() const
{
	ClearPixel(img, GetArea());
}
void
MBitmapBuffer::Fill(PixelType c) const
{
	FillSeq(img, GetArea(), c);
}

void
MBitmapBuffer::CopyToBuffer(BitmapPtr dst, const ROT rot, const SSize& ds,
							const SPoint& sp, const SPoint& dp, const SSize& sc) const
{
	if(~rot & 1 && dst && img)
	{
		if(rot)
			blitU(dst, ds,
			img, *this,
			sp, dp, sc);
		else
			blit(dst, ds,
			img, *this,
			sp, dp, sc);
	}
}


MBitmapBufferEx::MBitmapBufferEx(ConstBitmapPtr i, SDST w, SDST h) : MBitmapBuffer(i, w, h), imgAlpha(NULL)
{
	SetSize(w, h);
	if(i)
		memcpy(img, i, sizeof(PixelType) * GetArea());
}

void
MBitmapBufferEx::SetSize(SPOS w, SPOS h)
{
	if(w <= 0 || h <= 0)
	{
		if(img)
		{
			delete[] img;
			img = NULL;
		}
		if(imgAlpha)
		{
			delete[] imgAlpha;
			imgAlpha = NULL;
		}
	}
	else if(!img || Width != w || Height != h)
	{
		if(!img || w > Width || h > Height)
		{
			try
			{
				if(img)
					delete[] img;
				img = new PixelType[w * h];
				if(imgAlpha)
					delete[] imgAlpha;
				imgAlpha = new u8[w * h];
			}
			catch(std::bad_alloc&)
			{
				throw MLoggedEvent("Allocation failed @@ MBitmapBufferEx::SetSize(SPOS, SPOS);", 1);;
			}
		}
	}

	Width = w;
	Height = h;

	ClearImage();
}
void
MBitmapBufferEx::SetSizeSwap()
{
	SDST t = Width;

	Width = Height;
	Height = t;
	ClearImage();
}

void
MBitmapBufferEx::ClearImage() const
{
	const u32 t = GetArea();

	ClearPixel(img, t);
	ClearPixel(imgAlpha, t);
}

void
MBitmapBufferEx::CopyToBuffer(BitmapPtr dst, ROT rot, const SSize& ds,
							  const SPoint& sp, const SPoint& dp, const SSize& sc) const
{
	if(~rot & 1 && dst && img)
	{
		if(rot)
			blit2U(dst, ds,
			img, imgAlpha, *this,
			sp, dp, sc);
		else
			blit2(dst, ds,
			img, imgAlpha, *this,
			sp, dp, sc);
	}
}
void
MBitmapBufferEx::BlitToBuffer(BitmapPtr dst, ROT rot, const SSize& ds,
							  const SPoint& sp, const SPoint& dp, const SSize& sc) const
{
	if(~rot & 1 && dst && img)
	{
		if(rot)
			blitAlphaU(dst, ds,
			img, imgAlpha, *this,
			sp, dp, sc);
		else
			blitAlpha(dst, ds,
			img, imgAlpha, *this,
			sp, dp, sc);
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

﻿// YSLib::Service::YTextManager by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 17:48:09;
// UTime = 2010-8-4 18:55;
// Version = 0.4037;


#ifndef INCLUDED_YTMGR_H_
#define INCLUDED_YTMGR_H_

// YTextManager ：平台无关的基础文本管理服务。

#include "../Core/ystring.h"
#include "../Core/yftext.h"
#include <map>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

//文本缓冲区。
class MTextBuffer : private NonCopyable
{
private:
	const IndexType mlen; //最大长度（字符数）。

protected:
	//文本缓冲区的首地址和长度。
	uchar_t* const text;
	IndexType len;

	explicit
	MTextBuffer(IndexType);
	~MTextBuffer();

public:
	uchar_t&
	operator[](IndexType) ythrow(); //按下标返回字符，无运行期范围检查。

	DefGetter(IndexType, MaxLength, mlen) //取最大文本长度。
	DefGetter(SizeType, SizeOfBuffer, sizeof(uchar_t) * mlen) //取文本缓冲区的大小。
	DefGetter(uchar_t*, Ptr, text) //取文本缓冲区的指针。
	DefGetter(IndexType, Length, len) //取文本缓冲区的长度。
	IndexType
	GetPrevChar(IndexType o, uchar_t c); //从文本缓冲区下标 o （不含）起逆序查找字符 c 。返回结果的直接后继下标；查找失败时返回 0 。
	IndexType
	GetNextChar(IndexType o, uchar_t c); //从文本缓冲区下标 o （含）起顺序查找字符 c 。返回结果的下标；查找失败时返回缓冲区长度（o 原值小于缓冲区长度）或 o 原值（大于等于缓冲区长度时）。
	IndexType
	GetPrevNewline(IndexType o); //从文本缓冲区下标 o （不含）起逆序查找换行符。返回结果的直接后继下标；查找失败时返回 0 。
	IndexType
	GetNextNewline(IndexType o); //从文本缓冲区下标 o （含）起顺序查找字符 c 。返回结果的下标；查找失败时返回缓冲区长度（o 原值小于缓冲区长度）或 o 原值（大于等于缓冲区长度时）。

	uchar_t&
	at(IndexType) ythrow(std::out_of_range); //按下标返回字符，当越界时抛出 std::out_of_range 。

	void
	ClearText(); //清空缓冲区。

	bool
	Load(const uchar_t* s); //从起始地址 s 中读取连续的 mlen 个 uchar_t 字符。
	bool
	Load(const uchar_t* s, IndexType n); //从起始地址 s 中读取连续的 n 个 uchar_t 字符。
	IndexType
	Load(YTextFile& f); //从文本文件 f 中读取连续的 mlen 个字符（自动校验换行并转换为 Unix / Linux 格式），并返回成功读取的字符数。
	IndexType
	Load(YTextFile& f, IndexType n); //从文本文件 f 中读取连续的 n 个字符，并返回成功读取的字符数。

	IndexType
	LoadN(YTextFile& f, IndexType n); //从文本文件 f 中读取连续的 n 个字节，并返回成功读取的字符数。

	bool
	Output(uchar_t* d, IndexType p, IndexType n) const; //从偏移 p 个字符起输出 n 个 uchar_t 字符到 d 。

};

inline
MTextBuffer::~MTextBuffer()
{
	delete[] text;
}

inline IndexType
MTextBuffer::GetPrevNewline(IndexType o)
{
	return GetPrevChar(o, '\n');
}
inline IndexType
MTextBuffer::GetNextNewline(IndexType o)
{
	return GetNextChar(o, '\n');
}

inline void
MTextBuffer::ClearText()
{
	memset(text, 0, GetSizeOfBuffer());
}
inline bool
MTextBuffer::Load(const uchar_t* s)
{
	return Load(s, mlen);
}
inline IndexType
MTextBuffer::Load(YTextFile& f)
{
	return Load(f, mlen);
}


//文本缓冲块。
class MTextBlock : public MTextBuffer
{
public:
	typedef u16 BlockIndexType;

	BlockIndexType Index;

	MTextBlock(BlockIndexType, IndexType);
	virtual
	~MTextBlock();
};

inline
MTextBlock::MTextBlock(BlockIndexType i, IndexType tlen)
: MTextBuffer(tlen), Index(i)
{}
inline
MTextBlock::~MTextBlock()
{}


//文本缓冲块映射。
class MTextMap
{
public:
	typedef MTextBlock::BlockIndexType BlockIndexType;
	typedef std::map<BlockIndexType, MTextBlock*> MapType;

protected:
	MapType Map;

public:
	virtual
	~MTextMap();

	MTextBlock*
	operator[](const BlockIndexType&);
	void
	operator+=(MTextBlock&);
	bool
	operator-=(const BlockIndexType&);

	void
	clear();
};

inline
MTextMap::~MTextMap()
{
	clear();
}

inline MTextBlock*
MTextMap::operator[](const BlockIndexType& id)
{
	return Map[id];
}

inline void
MTextMap::operator+=(MTextBlock& item)
{
	Map.insert(std::make_pair(item.Index, &item));
}
inline bool
MTextMap::operator-=(const BlockIndexType& i)
{
	return Map.erase(i) != 0;
}


//文本文件块缓冲区。
class MTextFileBuffer : public MTextMap
{
public:
	typedef MTextMap::BlockIndexType BlockIndexType; //块索引类型。

	static const SizeType nBlockSize = 0x2000; //文本块容量。

	//只读文本循环迭代器类。
	class TextIterator
	{
		friend class MTextFileBuffer;

	public:
		typedef MTextFileBuffer ContainerType;
		typedef ContainerType::BlockIndexType BlockIndexType;

	private:
		MTextFileBuffer* pBuf;
		//文本读取位置。
		BlockIndexType blk;
		IndexType idx;

	public:
		explicit
		TextIterator(MTextFileBuffer&, BlockIndexType = 0, IndexType = 0) ythrow(); //指定文本读取位置初始化。

		TextIterator&
		operator++() ythrow();

		TextIterator&
		operator--() ythrow();

		uchar_t
		operator*() ythrow();

		TextIterator
		operator+(std::ptrdiff_t);

		TextIterator
		operator-(std::ptrdiff_t);

		friend bool
		operator==(const TextIterator&, const TextIterator&) ythrow();

		friend bool
		operator!=(const TextIterator&, const TextIterator&) ythrow();

		friend bool
		operator<(const TextIterator&, const TextIterator&) ythrow();

		friend bool
		operator>(const TextIterator&, const TextIterator&) ythrow();

		friend bool
		operator<=(const TextIterator&, const TextIterator&) ythrow();

		friend bool
		operator>=(const TextIterator&, const TextIterator&) ythrow();

		TextIterator&
		operator+=(std::ptrdiff_t);

		TextIterator&
		operator-=(std::ptrdiff_t);

		const uchar_t*
		GetTextPtr() const ythrow();
		IndexType
		GetBlockLength() const ythrow();
		IndexType
		GetBlockLength(BlockIndexType) const ythrow();
	};

	YTextFile& File; //文本文件。

private:
	const SizeType nTextSize; //文本区段长度。
	const BlockIndexType nBlock; //文本区块数。

public:
	explicit
	MTextFileBuffer(YTextFile&);

	MTextBlock&
	operator[](const BlockIndexType&);

	DefGetter(SizeType, TextSize, nTextSize)

	TextIterator
	begin() ythrow();

	TextIterator
	end() ythrow();
};

inline bool
operator!=(const MTextFileBuffer::TextIterator& lhs, const MTextFileBuffer::TextIterator& rhs) ythrow()
{
	return !(lhs == rhs);
}

inline bool
operator>(const MTextFileBuffer::TextIterator& lhs, const MTextFileBuffer::TextIterator& rhs) ythrow()
{
	return rhs < lhs;
}
inline bool
operator<=(const MTextFileBuffer::TextIterator& lhs, const MTextFileBuffer::TextIterator& rhs) ythrow()
{
	return !(rhs < lhs);
}

inline bool
operator>=(const MTextFileBuffer::TextIterator& lhs, const MTextFileBuffer::TextIterator& rhs) ythrow()
{
	return !(lhs < rhs);
}

inline MTextFileBuffer::TextIterator
MTextFileBuffer::TextIterator::operator-(std::ptrdiff_t o)
{
	return *this + -o;
}

inline MTextFileBuffer::TextIterator&
MTextFileBuffer::TextIterator::operator-=(std::ptrdiff_t o)
{
	return *this += -o;
}

inline IndexType
MTextFileBuffer::TextIterator::GetBlockLength() const ythrow()
{
	return GetBlockLength(blk);
}

inline MTextFileBuffer::TextIterator
MTextFileBuffer::begin() ythrow()
{
	return MTextFileBuffer::TextIterator(*this);
}

inline MTextFileBuffer::TextIterator
MTextFileBuffer::end() ythrow()
{
	return MTextFileBuffer::TextIterator(*this, nTextSize / nBlockSize);
}

YSL_END_NAMESPACE(Text)

YSL_END

#endif

﻿// YSLib::Adaptor::YReference by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-21 23:09:06 + 08:00;
// UTime = 2010-10-18 10:49 + 08:00;
// Version = 0.2219;


#ifndef INCLUDED_YREF_HPP_
#define INCLUDED_YREF_HPP_

// YReference ：用于提供指针和引用访问的间接访问类模块。

#include "yadaptor.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Design)

YSL_BEGIN_NAMESPACE(Policies)

YSL_BEGIN_NAMESPACE(Operations)

template<class P>
struct SmartPtr_Simple
{
	enum
	{
		destructiveCopy = false
	};

	SmartPtr_Simple()
	{}

	template<class P1>
	SmartPtr_Simple(const SmartPtr_Simple<P1>& rhs)
	{}

	static P
	Clone(const P& val)
	{
		return val;
	}

	bool
	Release(const P&)
	{
		return false;
	}

	static void
	Swap(SmartPtr_Simple&)
	{}
};

template<class P>
class SmartPtr_RefCounted
{
private:
	uintptr_t* pCount_;

protected:
	void
	AddRef()
	{
		++*pCount_;
	}

public:
	enum { destructiveCopy = false };

	SmartPtr_RefCounted()
	: pCount_(static_cast<uintptr_t*>(SmallObject<>::operator new(sizeof(uintptr_t))))
	{
	//	assert(pCount_!=0);
		*pCount_ = 1;
	}

	SmartPtr_RefCounted(const SmartPtr_RefCounted& rhs)
		: pCount_(rhs.pCount_)
	{}

	template<typename P1>
	SmartPtr_RefCounted(const SmartPtr_RefCounted<P1>& rhs)
		: pCount_(reinterpret_cast<const SmartPtr_RefCounted&>(rhs).pCount_)
	{}

	P
	Clone(const P& val)
	{
		AddRef();
		return val;
	}

	bool
	Release(const P&)
	{
		if(!--*pCount_)
		{
			SmallObject<>::operator delete(pCount_, sizeof(uintptr_t));
			pCount_ = NULL;
			return true;
		}
		return false;
	}

	void
	Swap(SmartPtr_RefCounted& rhs)
	{
		std::swap(pCount_, rhs.pCount_);
	}
};

YSL_END_NAMESPACE(Operations)

YSL_END_NAMESPACE(Policies)

YSL_END_NAMESPACE(Design)


template<typename T,
	template<class> class OP = Design::Policies::Operations::SmartPtr_RefCounted,
	class CP = AllowConversion,
	template<class> class KP = RejectNull,
	template<class> class SP = DefaultSPStorage,
	typename SPType = SmartPtr<T, OP, CP, KP, SP> >
class GHResource : public SPType //资源指针类。
{
public:
	GHResource(T* p = NULL) : SPType(p) {};
	GHResource(T& rhs) : SPType(rhs) {}
	GHResource(RefToValue<GHResource> rhs) : SPType(rhs) {}

	operator RefToValue<GHResource>()
	{
		return RefToValue<GHResource>(*this);
	}

	T*
	operator->() const
	{
		return GHResource<T>::GetPointer(*this);
	}

	GHResource&
	operator=(T& rhs)
	{
		GHResource temp(rhs);
		temp.Swap(*this);
		return *this;
	}

	T*
	GetPtr() const
	{
		return operator->();
	}

	void
	Swap(GHResource& rhs)
	{
		Swap(rhs);
	}
};


template<class _type>
inline _type*
GetPointer(GHResource<_type> h)
{
	return h.GetPtr();
}


#define YHandleOP Design::Policies::Operations::SmartPtr_Simple
//句柄不会被自动回收，需要手动释放。
#define YDelete(h) delete h


template<typename T,
	template<class> class OP = YHandleOP,
	class CP = AllowConversion,
	template<class> class KP = RejectNull,
	template<class> class SP = DefaultSPStorage,
	typename SPType = SmartPtr<T, OP, CP, KP, SP> >
class GHHandle : public SPType //句柄类。
{
public:
	GHHandle(T* p = NULL) : SPType(p) {}
	GHHandle(T& rhs) : SPType(rhs) {}
	GHHandle(RefToValue<GHHandle> rhs) : SPType(rhs) {}
	template<class C>
	explicit
	GHHandle(GHHandle<C>& h) : SPType(static_cast<T*>(GHHandle<C>::GetPointer(h))) {}

	operator RefToValue<GHHandle>()
	{
		return RefToValue<GHHandle>(*this);
	}

	T*
	operator->() const
	{
		return GHHandle<T>::GetPointer(*this);
	}

	GHHandle&
	operator=(T& rhs)
	{
		GHHandle temp(rhs);
		temp.Swap(*this);
		return *this;
	}

	T*
	GetPtr() const
	{
		return operator->();
	}

	void
	Swap(GHHandle& rhs)
	{
		Swap(rhs);
	}
};


#undef YHandleOP


template<typename _type>
inline _type*
GetPointer(_type* h)
{
	return h;
}

#ifndef YSL_USE_SIMPLE_HANDLE

template<typename _tReference>
inline _tReference*
GetPointer(GHHandle<_tReference> h)
{
	return h.GetPtr();
}

template<typename _type, typename _tReference>
inline _type
handle_cast(GHHandle<_tReference> h)
{
	return reinterpret_cast<_type>(GHHandle<_tReference>::GetPointer(h));
}

#endif

YSL_END

#endif


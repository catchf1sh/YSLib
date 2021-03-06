﻿/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YObject.h
\ingroup Core
\brief 平台无关的基础对象。
\version r3904
\author FrankHB <frankhb1989@gmail.com>
\since build 561
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2015-05-03 08:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YObject
*/


#ifndef YSL_INC_Core_yobject_h_
#define YSL_INC_Core_yobject_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities
#include <ystdex/any.h> // for ystdex::any_ops::holder, ystdex::any;
#include <ystdex/examiner.hpp> // for ystdex::examiners::equal_examiner;

namespace YSLib
{

//特征类策略：对象类型标签模板。

/*!
\brief 指定对参数指定类型的成员具有所有权的标签。
\since build 218
*/
template<typename>
struct OwnershipTag
{};


/*!
\brief 指示转移的标记。
\since build 340
*/
struct MoveTag
{};


/*!
\brief 指示指针的标记。
\since build 340
*/
struct PointerTag
{};


/*!
\brief 标签类型元运算。
\since build 218
*/
template<class _tOwner, typename _type>
struct HasOwnershipOf : std::integral_constant<bool,
	std::is_base_of<OwnershipTag<_type>, _tOwner>::value>
{};


/*!
\brief 带等于接口的动态泛型持有者接口。
\sa ystdex::any_holder
\since build 332
*/
DeclDerivedI(YF_API, IValueHolder, ystdex::any_ops::holder)
	DeclIEntry(bool operator==(const IValueHolder&) const)
EndDecl


//! \since build 454
//@{
/*!
\brief 判断动态泛型的持有值是否相等。
\return 若可通过 == 比较则为比较结果，否则为 true 。
*/
//@{
template<typename _type1, typename _type2>
struct HeldEqual : private ystdex::examiners::equal_examiner
{
	using ystdex::examiners::equal_examiner::are_equal;
};

template<typename _type1, typename _type2>
struct HeldEqual<weak_ptr<_type1>, weak_ptr<_type2>>
{
	static inline bool
	are_equal(const weak_ptr<_type1>& x, const weak_ptr<_type2>& y)
	{
		return x == y;
	}
};

template<typename _type1, typename _type2, typename _type3, typename _type4>
struct HeldEqual<pair<_type1, _type2>, pair<_type3, _type4>>
{
	static yconstfn bool
	are_equal(const pair<_type1, _type2>& x, const pair<_type3, _type4>& y)
	{
		return x.first == y.first && x.second == y.second;
	}
};
//@}


/*!
\brief 判断动态泛型的持有值是否相等。
\note 不直接使用 ystdex::examiners::equal_examiner 判断，
	因为可以有其它重载操作符。
\since build 454
*/
template<typename _type1, typename _type2>
yconstfn bool
AreEqualHeld(const _type1& x, const _type2& y)
{
	return HeldEqual<_type1, _type2>::are_equal(x, y);
}
//@}


/*!
\brief 带等于接口的值类型动态泛型持有者。
\note 成员命名参照 ystdex::value_holder 。
\note 比较使用 AreEqualHeld ，可有 ADL 重载。
\sa ystdex::value_holder
\sa AreEqualHeld
\since build 332
*/
template<typename _type>
class ValueHolder : implements IValueHolder
{
	static_assert(std::is_object<_type>(), "Non-object type found.");
	static_assert(!ystdex::is_cv<_type>(), "Cv-qualified type found.");

public:
	//! \since build 352
	using value_type = _type;

protected:
	//! \since build 348
	mutable _type held;

public:
	ValueHolder(const _type& value)
		: held(value)
	{}
	//! \since build 340
	ValueHolder(_type&& value)
		: held(std::move(value))
	{}
	DefDeCopyCtor(ValueHolder)
	DefDeMoveCtor(ValueHolder)

	//! \since build 353
	DefDeCopyAssignment(ValueHolder)
	//! \since build 555
	DefDeMoveAssignment(ValueHolder)

	PDefHOp(bool, ==, const IValueHolder& obj) const ImplI(IValueHolder)
		ImplRet(AreEqualHeld(held, static_cast<const ValueHolder&>(obj).held))

	//! \since build 409
	DefClone(const ImplI(IValueHolder), ValueHolder)

	//! \since build 348
	PDefH(void*, get, ) const ImplI(IValueHolder)
		ImplRet(std::addressof(held))

	//! \since build 340
	PDefH(const std::type_info&, type, ) const ynothrow ImplI(IValueHolder)
		ImplRet(typeid(_type))
};


/*!
\brief 带等于接口的指针类型动态泛型持有者。
\tparam _tPointer 智能指针类型。
\pre _tPointer 具有 _type 对象所有权。
\note 成员命名参照 ystdex::pointer_holder 。
\note 比较使用 AreEqualHeld ，可有 ADL 重载。
\sa ystdex::pointer_holder
\sa AreEqualHeld
\since build 555
*/
template<typename _type, class _tPointer = std::unique_ptr<_type>>
class PointerHolder : implements IValueHolder
{
	//! \since build 332
	static_assert(std::is_object<_type>(), "Invalid type found.");

public:
	//! \since build 352
	using value_type = _type;
	using holder_pointer = _tPointer;
	using pointer = typename holder_pointer::pointer;

protected:
	holder_pointer p_held;

public:
	//! \brief 取得所有权。
	PointerHolder(pointer value)
		: p_held(value)
	{}
	//! \since build 352
	//@{
	PointerHolder(const PointerHolder& h)
		: PointerHolder(h.p_held ? new _type(*h.p_held) : nullptr)
	{}
	DefDeMoveCtor(PointerHolder)
	//@}

	//! \since build 353
	DefDeCopyAssignment(PointerHolder)
	DefDeMoveAssignment(PointerHolder)

	//! \since build 332
	PDefHOp(bool, ==, const IValueHolder& obj) const ImplI(IValueHolder)
		ImplRet(AreEqualHeld(*p_held,
			Deref(static_cast<const PointerHolder&>(obj).p_held)))

	//! \since build 409
	DefClone(const ImplI(IValueHolder), PointerHolder)

	//! \since build 348
	PDefH(void*, get, ) const ImplI(IValueHolder)
		ImplRet(p_held.get())

	//! \since build 340
	PDefH(const std::type_info&, type, ) const ynothrow ImplI(IValueHolder)
		ImplRet(p_held ? typeid(_type) : typeid(void))
};


/*!
\brief 值类型对象类。
\pre 满足 CopyConstructible 。
\warning 非虚析构。
\warning 若修改子节点的 name 则行为未定义。
\since build 217

具有值语义和深复制语义的对象。
*/
class YF_API ValueObject
{
public:
	//! \since build 332
	ystdex::any content;

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	\since build 296
	*/
	DefDeCtor(ValueObject)
	/*!
	\brief 构造：使用对象引用。
	\pre obj 可作为转移构造参数。
	\since build 448
	*/
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_ctor_t<ValueObject, _type>)>
	ValueObject(_type&& obj)
		: content(ystdex::any_ops::holder_tag(), make_unique<ValueHolder<
		ystdex::decay_t<_type>>>(yforward(obj)))
	{}
	/*!
	\brief 构造：使用对象指针。
	\note 得到包含指针指向的指定对象的实例，并获得所有权。
	\note 使用 PointerHolder 管理资源（默认使用 delete 释放资源）。
	\since build 340
	*/
	template<typename _type>
	ValueObject(_type* p, PointerTag)
		: content(ystdex::any_ops::holder_tag(), make_unique<PointerHolder<
		_type>>(p))
	{}
	/*!
	\brief 构造：使用对象 unique_ptr 指针。
	\note 得到包含指针指向的指定对象的实例，并获得所有权。
	\note 使用 PointerHolder 管理资源（默认使用 delete 释放资源）。
	\since build 450
	*/
	template<typename _type>
	ValueObject(unique_ptr<_type>&& p, PointerTag)
		: ValueObject(p.get(), PointerTag())
	{
		p.release();
	}
	/*!
	\brief 复制构造：默认实现。
	\since build 332
	*/
	DefDeCopyCtor(ValueObject)
	/*!
	\brief 转移构造：默认实现。
	\since build 332
	*/
	DefDeMoveCtor(ValueObject)
	/*!
	\brief 析构：默认实现。
	\since build 332
	*/
	DefDeDtor(ValueObject)

	//! \since build 332
	//@{
	DefDeCopyAssignment(ValueObject)
	DefDeMoveAssignment(ValueObject)
	//@}

	/*!
	\brief 判断是否为空或非空。
	\since build 320
	*/
	DefBoolNeg(explicit, content.get_holder())

	bool
	operator==(const ValueObject&) const;

private:
	/*!
	\brief 取指定类型的对象。
	\tparam _type 指定类型。
	\pre 断言： bool(content) && content.type() == typeid(_type) 。
	\since build 337
	*/
	template<typename _type>
	inline _type&
	GetMutableObject() const
	{
		YAssertNonnull(content);
		YAssert(content.type() == typeid(_type), "Invalid type found.");
		return Deref(static_cast<_type*>(content.get()));
	}

public:
	template<typename _type>
	inline _type&
	GetObject()
	{
		return GetMutableObject<_type>();
	}
	template<typename _type>
	inline const _type&
	GetObject() const
	{
		return GetMutableObject<_type>();
	}
	//! \since build 340
	DefGetter(const ynothrow, const std::type_info&, Type, content.type())

	/*!
	\brief 访问指定类型对象。
	\exception std::bad_cast 空实例或类型检查失败 。
	\since build 334
	*/
	//@{
	template<typename _type>
	inline _type&
	Access()
	{
		return ystdex::any_cast<_type&>(content);
	}
	template<typename _type>
	inline const _type&
	Access() const
	{
		return ystdex::any_cast<const _type&>(content);
	}
	//@}

	/*!
	\brief 访问指定类型对象指针。
	\since build 398
	*/
	//@{
	template<typename _type>
	inline _type*
	AccessPtr() ynothrow
	{
		return ystdex::any_cast<_type*>(&content);
	}
	template<typename _type>
	inline const _type*
	AccessPtr() const ynothrow
	{
		return ystdex::any_cast<const _type*>(&content);
	}
	//@}

	/*
	\brief 清除。
	\post <tt>*this == ValueObject()</tt> 。
	\since build 296
	*/
	PDefH(void, Clear, ) ynothrow
		ImplExpr(content.clear())

	/*!
	\brief 交换。
	\since build 409
	*/
	PDefH(void, swap, ValueObject& vo) ynothrow
		ImplExpr(content.swap(vo.content))
};

/*!
\relates ValueObject
\since build 409
*/
inline DefSwap(ynothrow, ValueObject)


/*!
\brief 依赖项模板。
\tparam _type 被依赖的对象类型，需能被无参数构造。
\tparam _tOwnerPointer 依赖所有者指针类型。
\warning 依赖所有者指针需要实现所有权语义，
	否则出现无法释放资源导致内存泄漏或其它非预期行为。
\since build 195
\todo 线程模型及安全性。

基于被依赖的默认对象，可通过写时复制策略创建新对象；可能为空。
*/
template<typename _type, class _tOwnerPointer = shared_ptr<_type>>
class GDependency
{
public:
	using DependentType = _type;
	using PointerType = _tOwnerPointer;
	using ConstReferenceType = decltype(*PointerType());
	using ReferentType = ystdex::remove_const_t<ystdex::remove_reference_t<
		ConstReferenceType>>;
	using ReferenceType = ReferentType&;

private:
	PointerType ptr;

public:
	inline
	GDependency(PointerType p = PointerType())
		: ptr(p)
	{
		GetCopyOnWritePtr();
	}

	DefDeCopyAssignment(GDependency)
	DefDeMoveAssignment(GDependency)

	DefCvt(const ynothrow, ConstReferenceType, *ptr)
	DefCvt(ynothrow, ReferenceType, *ptr)
	DefCvt(const ynothrow, bool, bool(ptr))

	DefGetter(const ynothrow, ConstReferenceType, Ref,
		operator ConstReferenceType())
	DefGetter(ynothrow, ReferenceType, Ref, operator ReferenceType())
	DefGetter(ynothrow, ReferenceType, NewRef, *GetCopyOnWritePtr())

	//! \post 返回值非空。
	PointerType
	GetCopyOnWritePtr()
	{
		if(!ptr)
			ptr = PointerType(new DependentType());
		else if(!ptr.unique())
			ptr = PointerType(CloneNonpolymorphic(ptr));
		return Nonnull(ptr);
	}

	inline
	void Reset()
	{
		reset(ptr);
	}
};


/*!
\brief 范围模块类。
\warning 非虚析构。
\since build 193
*/
template<typename _type>
class GMRange
{
public:
	using ValueType = _type;

protected:
	ValueType max_value; //!< 最大取值。
	ValueType value; //!< 值。

	/*!
	\brief 构造：使用指定最大取值和值。
	*/
	GMRange(ValueType m, ValueType v)
		: max_value(m), value(v)
	{}

public:
	DefGetter(const ynothrow, ValueType, MaxValue, max_value)
	DefGetter(const ynothrow, ValueType, Value, value)
};

} // namespace YSLib;

#endif


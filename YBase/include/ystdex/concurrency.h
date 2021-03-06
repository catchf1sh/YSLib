﻿/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file concurrency.h
\ingroup YStandardEx
\brief 并发操作。
\version r381
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2014-07-21 18:57:13 +0800
\par 修改时间:
	2015-04-29 01:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Concurrency
*/


#ifndef YB_INC_ystdex_concurrency_h_
#define YB_INC_ystdex_concurrency_h_ 1

#include "pseudo_mutex.h" // for result_of_t, std::declval,
//	std::make_shared, threading::unlock_delete, noncopyable;
#include <functional> // for std::bind, std::function;
#include <future> // for std::packaged_task, std::future;
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace ystdex
{

/*!
\brief 解锁删除器：默认使用标准库互斥量和锁。
\sa threading::unlock_delete
\since build 590
*/
template<class _tMutex = std::mutex,
	class _tLock = std::unique_lock<_tMutex>>
using unlock_delete = threading::unlock_delete<_tMutex, _tLock>;


/*!
\brief 独占所有权的锁定指针：默认使用标准库互斥量和锁。
\sa threading::locked_ptr
\since build 551
*/
template<typename _type, class _tMutex = std::mutex,
	class _tLock = std::unique_lock<_tMutex>>
using locked_ptr = threading::locked_ptr<_type, _tMutex, _tLock>;


/*!
\brief 转换 \c thread::id 为字符串表示。
\note 使用 \c operator<< 实现。
\since build 543
*/
YB_API std::string
to_string(const std::thread::id&);


//! \since build 358
template<typename _fCallable, typename... _tParams>
using packed_task_t
	= std::packaged_task<result_of_t<_fCallable&&(_tParams&&...)>()>;


//! \since build 359
//@{
template<typename _fCallable, typename... _tParams>
packed_task_t<_fCallable&&, _tParams&&...>
pack_task(_fCallable&& f, _tParams&&... args)
{
	return packed_task_t<_fCallable&&, _tParams&&...>(std::bind(yforward(f),
		yforward(args)...));
}

template<typename _fCallable, typename... _tParams>
std::shared_ptr<packed_task_t<_fCallable&&, _tParams&&...>>
pack_shared_task(_fCallable&& f, _tParams&&... args)
{
	return std::make_shared<packed_task_t<_fCallable&&, _tParams&&...>>(
		std::bind(yforward(f), yforward(args)...));
}
//@}


/*!
\brief 线程池。
\note 除非另行约定，所有公开成员函数线程安全。
\note 未控制线程队列的长度。
\since build 520
*/
class YB_API thread_pool : private noncopyable
{
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks{};
	mutable std::mutex queue_mutex{};
	std::condition_variable condition{};
	/*!
	\brief 停止状态。
	\note 仅在析构时设置停止。
	\since build 538
	*/
	bool stopped = {};

public:
	/*!
	\brief 初始化：使用指定的初始化和退出回调指定数量的工作线程。
	\note 若回调为空则忽略。
	\warning 回调的执行不提供顺序和并发安全保证。
	\since build 543
	*/
	thread_pool(size_t, std::function<void()> = {}, std::function<void()> = {});
	/*!
	\brief 析构：设置停止状态并等待所有执行中的线程结束。
	\note 断言设置停止状态时不抛出 \c std::system_error 。
	\note 可能阻塞。忽略每个线程阻塞的 \c std::system_error 。
	\note 无异常抛出：断言。
	\since build 552
	*/
	~thread_pool() ynothrow;

	//! \see wait_to_enqueue
	template<typename _fCallable, typename... _tParams>
	std::future<result_of_t<_fCallable&&(_tParams&&...)>>
	enqueue(_fCallable&& f, _tParams&&... args)
	{
		return wait_to_enqueue([](std::unique_lock<std::mutex>&) ynothrow{},
			yforward(f), yforward(args)...);
	}

	size_t
	size() const;

	/*!
	\warning 非线程安全。
	\since build 552
	*/
	size_t
	size_unlocked() const ynothrow
	{
		return tasks.size();
	}

	/*!
	\brief 等待操作进入队列。
	\param f 准备进入队列的操作
	\param args 进入队列操作时的参数。
	\param waiter 等待操作。
	\pre waiter 调用后满足条件变量后置条件；断言：持有锁。
	\warning 需要确保未被停止（未进入析构），否则不保证任务被运行。
	\warning 使用非递归锁，等待时不能再次锁定。
	\since build 538
	*/
	template<typename _fWaiter, typename _fCallable, typename... _tParams>
	std::future<result_of_t<_fCallable&&(_tParams&&...)>>
	wait_to_enqueue(_fWaiter waiter, _fCallable&& f, _tParams&&... args)
	{
		const auto
			task(ystdex::pack_shared_task(yforward(f), yforward(args)...));
		auto res(task->get_future());

		{
			std::unique_lock<std::mutex> lck(queue_mutex);

			waiter(lck);
			yassume(lck.owns_lock());
			tasks.push([task]{
				(*task)();
			});
		}
		condition.notify_one();
		return std::move(res);
	}
};


/*!
\brief 任务池：带有队列大小限制的线程池。
\note 除非另行约定，所有公开成员函数线程安全。
\since build 538
\todo 允许调整队列大小限制。
*/
class YB_API task_pool : private thread_pool
{
private:
	size_t max_tasks;
	std::condition_variable enqueue_condition{};

public:
	/*!
	\brief 初始化：使用指定的初始化和退出回调指定数量的工作线程和最大任务数。
	\sa thread_pool::thread_pool
	\since build 543
	*/
	task_pool(size_t n, std::function<void()> on_enter = {},
		std::function<void()> on_exit = {})
		: thread_pool(std::max<size_t>(n, 1), on_enter, on_exit),
		max_tasks(std::max<size_t>(n, 1))
	{}

	//! \warning 非线程安全。
	bool
	can_enqueue_unlocked() const ynothrow
	{
		return size_unlocked() < max_tasks;
	}

	size_t
	get_max_task_num() const ynothrow
	{
		return max_tasks;
	}

	/*!
	\brief 重置线程池。
	\note 阻塞等待当前所有任务完成后重新创建。
	*/
	void
	reset();

	using thread_pool::size;

	//! \since build 543
	//@{
	template<typename _func, typename _fCallable, typename... _tParams>
	auto
	poll(_func poller, _fCallable&& f, _tParams&&... args)
		-> decltype(enqueue(yforward(f), yforward(args)...))
	{
		return wait_to_enqueue([=](std::unique_lock<std::mutex>& lck){
			enqueue_condition.wait(lck, [=]{
				return poller() && can_enqueue_unlocked();
			});
		}, yforward(f), yforward(args)...);
	}

	template<typename _func, typename _tDuration, typename _fCallable,
		typename... _tParams>
	auto
	poll_for(_func poller, const _tDuration& duration, _fCallable&& f,
		_tParams&&... args) -> decltype(enqueue(yforward(f), yforward(args)...))
	{
		return wait_to_enqueue([=](std::unique_lock<std::mutex>& lck){
			enqueue_condition.wait_for(lck, duration, [=]{
				return poller() && can_enqueue_unlocked();
			});
		}, yforward(f), yforward(args)...);
	}

	template<typename _func, typename _tTimePoint, typename _fCallable,
		typename... _tParams>
	auto
	poll_until(_func poller, const _tTimePoint& abs_time,
		_fCallable&& f, _tParams&&... args)
		-> decltype(enqueue(yforward(f), yforward(args)...))
	{
		return wait_to_enqueue([=](std::unique_lock<std::mutex>& lck){
			enqueue_condition.wait_until(lck, abs_time, [=]{
				return poller() && can_enqueue_unlocked();
			});
		}, yforward(f), yforward(args)...);
	}
	//@}

	//! \since build 539
	template<typename _fCallable, typename... _tParams>
	inline auto
	wait(_fCallable&& f, _tParams&&... args)
		-> decltype(enqueue(yforward(f), yforward(args)...))
	{
		return poll([]{
			return true;
		}, yforward(f), yforward(args)...);
	}

	template<typename _tDuration, typename _fCallable, typename... _tParams>
	inline auto
	wait_for(const _tDuration& duration, _fCallable&& f, _tParams&&... args)
		-> decltype(enqueue(yforward(f), yforward(args)...))
	{
		return poll_for([]{
			return true;
		}, duration, yforward(f), yforward(args)...);
	}

	template<typename _fWaiter, typename _fCallable, typename... _tParams>
	auto
	wait_to_enqueue(_fWaiter waiter, _fCallable&& f, _tParams&&... args)
		-> decltype(enqueue(yforward(f), yforward(args)...))
	{
		return thread_pool::wait_to_enqueue(
			[=](std::unique_lock<std::mutex>& lck){
			while(!can_enqueue_unlocked())
				waiter(lck);
		}, yforward(f), yforward(args)...);
	}

	//! \since build 539
	template<typename _tTimePoint, typename _fCallable, typename... _tParams>
	inline auto
	wait_until(const _tTimePoint& abs_time, _fCallable&& f, _tParams&&... args)
		-> decltype(enqueue(yforward(f), yforward(args)...))
	{
		return poll_until([]{
			return true;
		}, abs_time, yforward(f), yforward(args)...);
	}
};

} // namespace ystdex;

#endif


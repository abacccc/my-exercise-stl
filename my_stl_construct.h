#ifndef _MY_STL_CONSTRUCT_H
#define _MY_STL_CONSTRUCT_H

#include<new>
#include"my_type_traits.h"

namespace mySTL
{
	template<typename T1,typename T2>
	inline void construct(T1* p, const T2& value)
	{
		new (p)T1(value);
		/*
		placement new：只是operator new重载的一个版本。它并不分配内存，只是返回指向已经分配好的某段内存的一个指针。因此不能删除它，但需要调用对象的析构函数。
		在很多情况下，placement new的使用方法和其他普通的new有所不同。这里提供了它的使用步骤。

		第一步  缓存提前分配

		有三种方式：

		1.为了保证通过placement new使用的缓存区的memory alignment(内存队列)正确准备，使用普通的new来分配它：在堆上进行分配
		class Task ;
		char * buff = new [sizeof(Task)]; //分配内存
		(请注意auto或者static内存并非都正确地为每一个对象类型排列，所以，你将不能以placement new使用它们。)

		2.在栈上进行分配
		class Task ;
		char buf[N*sizeof(Task)]; //分配内存

		3.还有一种方式，就是直接通过地址来使用。(必须是有意义的地址)
		void* buf = reinterpret_cast<void*> (0xF00F);

		第二步：对象的分配

		在刚才已分配的缓存区调用placement new来构造一个对象。
		Task *ptask = new (buf) Task

		第三步：使用

		按照普通方式使用分配的对象：

		ptask->memberfunction();

		ptask-> member;

		//...

		第四步：对象的析构

		一旦你使用完这个对象，你必须调用它的析构函数来毁灭它。按照下面的方式调用析构函数：
		ptask->~Task(); //调用外在的析构函数

		第五步：释放

		你可以反复利用缓存并给它分配一个新的对象（重复步骤2，3，4）如果你不打算再次使用这个缓存，你可以象这样释放它：delete [] buf;

		跳过任何步骤就可能导致运行时间的崩溃，内存泄露，以及其它的意想不到的情况。如果你确实需要使用placement new，请认真遵循以上的步骤。
		*/
	}

	template<typename T>
	inline void destroy(T* pointer)
	{
		pointer->~T();
	}

	template<typename ForwardIterator>
	inline void destory(ForwardIterator first, ForwardIterator last)
	{
		__destory(first, last, value_type(first));
	}

	template<typename ForwardIterator,typename T>
	inline void __destory(ForwardIterator first, ForwardIterator last, T*)
	{
		typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
		__destory_aux(first, last, trivial_destructor());  //判断是否有trivial destructor
	}

	template<typename ForwardIterator>
	inline void __destory_aux(ForwardIterator first, ForwardIterator last, __false_type)
	{
		for (; first < last; ++first)
		{
			destory(&*first);
		}
	}
	
	template<typename ForwardIterator>
	inline void __destory_aux(ForwardIterator first, ForwardIterator last, __true_type)
	{

	}

	template<>
	inline void destory(char*, char*){}
	template<>
	inline void destory(wchar_t*, wchar_t*){}
}

#endif
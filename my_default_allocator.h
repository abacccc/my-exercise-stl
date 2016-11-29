//max函数需要重写，暂时用algorithm中的已有函数代替

#ifndef _MY_DEFAULT_ALLOCATOR
#define _MY_DEFAULT_ALLOCATOR

#include<new.h>
#include<cstddef>
#include<cstdlib>
#include<climits>
#include<iostream>
#include<algorithm> //需要替换


namespace mySTL
{
	template<typename T>
	inline T* allocate(ptrdiff_t size, T*) //标准库类型(library type)ptrdiff_t 与 size_t 类型一样,ptrdiff_t 也是一种与机器相关的类型,在 cstddef 头文件中定义。size_t 是unsigned 类型,而 ptrdiff_t 则是 signed 整型
	{
		set_new_handler(0);
		/*
		定义在new头文件中
		函数原型std::new_handler set_new_handler( std::new_handler new_p )，std::new_handler是一个函数指针类型（函数没有形参且返回void），内存分配失败时调用
		内存不够直接抛出异常
		*/
		T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
		/*
		双冒号意为全局
		operator new只分配所要求的空间，不调用相关对象的构造函数。当无法满足所要求分配的空间时，则
		->如果有new_handler，则调用new_handler，否则
		->如果没要求不抛出异常（以nothrow参数表达），则执行bad_alloc异常，否则
		->返回0
		*/
		if (tmp == 0)
		{
			std::cerr << "out of memory" << endl;
			exit(1);
			/*
			exit（0）：正常运行程序并退出程序；
			exit（1）：非正常运行导致退出程序；
			*/
		}
		return tmp;
	}


	template<typename T>
	inline void deallocate(T* buffer)
	{
		::operator delete(buffer);
	}

	template<typename T>
	class allocator
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		pointer allocate(size_type n)
		{
			return mySTL::allocate((difference_type)n, (pointer)0);
		}

		void deallocate(pointer p)
		{
			deallocate(p);
		}

		pointer address(reference x)
		{
			return (pointer)&x;
		}

		const_pointer const_address(const_reference x)
		{
			return (const_pointer)&x;
		}

		size_type init_page_size()
		{
			std::max(size_type(1), size_type(4096 / sizeof(T)));
		}

		size_type max_size() const
		{
			return std::max(size_type(1), size_type(UINT_MAX / sizeof(T)));//UINT_MAX在climits中
		}
	};

	template<>
	class allocator<void>
	{
	public:
		typedef void* pointer;
	};
}


#endif
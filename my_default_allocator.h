//max������Ҫ��д����ʱ��algorithm�е����к�������

#ifndef _MY_DEFAULT_ALLOCATOR
#define _MY_DEFAULT_ALLOCATOR

#include<new.h>
#include<cstddef>
#include<cstdlib>
#include<climits>
#include<iostream>
#include<algorithm> //��Ҫ�滻


namespace mySTL
{
	template<typename T>
	inline T* allocate(ptrdiff_t size, T*) //��׼������(library type)ptrdiff_t �� size_t ����һ��,ptrdiff_t Ҳ��һ���������ص�����,�� cstddef ͷ�ļ��ж��塣size_t ��unsigned ����,�� ptrdiff_t ���� signed ����
	{
		set_new_handler(0);
		/*
		������newͷ�ļ���
		����ԭ��std::new_handler set_new_handler( std::new_handler new_p )��std::new_handler��һ������ָ�����ͣ�����û���β��ҷ���void�����ڴ����ʧ��ʱ����
		�ڴ治��ֱ���׳��쳣
		*/
		T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
		/*
		˫ð����Ϊȫ��
		operator newֻ������Ҫ��Ŀռ䣬��������ض���Ĺ��캯�������޷�������Ҫ�����Ŀռ�ʱ����
		->�����new_handler�������new_handler������
		->���ûҪ���׳��쳣����nothrow����������ִ��bad_alloc�쳣������
		->����0
		*/
		if (tmp == 0)
		{
			std::cerr << "out of memory" << endl;
			exit(1);
			/*
			exit��0�����������г����˳�����
			exit��1�������������е����˳�����
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
			return std::max(size_type(1), size_type(UINT_MAX / sizeof(T)));//UINT_MAX��climits��
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
#ifndef _MY_STL_ALGORITHM_H
#define _MY_STL_ALGORITHM_H

#include"my_type_traits.h"

namespace mySTL
{

	//一下为插入排序
	template<typename RandomAccessIterator, typename T>
	void __unguarded_linear_insert(RandomAccessIterator last, T value)
	{
		RandomAccessIterator next = last;
		--next;
		while (value < *next)
		{
			*last = *next;
			last = next;
			--next;
		}
		*last = value;
	}

	template<typename RandomAccessIterator, typename T>
	inline void __linear_insert(RandomAccessIterator first, RandomAccessIteratro last, T*)
	{
		T value = *last;
		if (value < *first)
		{
			//将所有元素后移一位，用copybackward
		}
		else
		{
			__unguarded_linear_insert(last, value);
		}
	}

	template<typename RandomAccessIterator>
	void __insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
	{
		if (first == last)
		{
			return;
		}
		for (RandomAccessIterator i = first + 1; i != last; ++i)
		{
			__linear_insert(first, i, value_type(first));
		}
	}


	//一下为快速排序
	template<typename T>
	inline const T& __median(const T& a, const T& b, const T& c)
	{
		if (a < b)
		{
			if (b < c)
			{
				return b;
			}
			else if (a < c)
			{
				return c;
			}
			else
			{
				return a;
			}
		}
		else if (a < c)
		{
			return a;
		}
		else if (b < c)
		{
			return c;
		}
		else
		{
			return b;
		}
	}

	template<typename RandomAccessIterator, typename T>
	RandomAccessIterator __unguarded_partition(RandomAccessIterator first, RandomAccessIterator last, T pivot)
	{
		while (true)
		{
			while (*first < pivot)
			{
				++first;
			}
			--last;
			while (pivot < *last)
			{
				--last;
			}

			if (!(first < last))
			{
				return first;
			}
			T tmp;
			tmp = *first;
			*first = *last;
			*last = tmp;
			++first;
		}
	}

	template<typename Size>
	inline Size __lg(Size n)
	{
		Size k;
		for (k = 0; n > 1; n >>= 1)
		{
			++k;
		}
		return k;
	}

	const int __stl_threshold = 16;

	template<typename RandomAccessIterator,typename T,typename Size>
	void __introsort_loop(RandomAccessIterator first, RandomAccessIterator last, T*, Size depth_limit)
	{
		while (last - first > __stl_threshold)
		{
			if (depth_limit == 0)//递归层次过深
			{
				//堆排序，待补充
				return;
			}

			--depth_limit;
			//需补充
			RandomAccessIterator cut = __unguarded_partition(__first, __last, T(__median(*__first, *(__first + (__last - __first) / 2), *(__last - 1))));
			__introsort_loop(__cut, __last, (_Tp*)0, __depth_limit);
			last = cut;
		}
		
	}






}



#endif
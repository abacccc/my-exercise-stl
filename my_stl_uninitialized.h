#ifndef _MY_STL_UNINITIALIZED_H
#define _MY_STL_UNINITIALIZED_H

#include"my_type_traits.h"
#include"my_stl_iterator.h"
#include"my_stl_construct.h"
#include"my_stl_algorithm.h"

//≈–∂œ «∑ÒÕÍ≥…my_stl_algorithm.h
#ifndef _MY_STL_ALGORITHM_H
#	include<algorithm>
#endif

namespace mySTL
{
	//uniitialized_fill_n
	template<typename ForwardIterator, typename Size, typename T>
	inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
	{
		return std::fill_n(first, n, x);
	}

	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
	{
		ForwardIterator cur = first;
		try
		{
			for (; n > 0; --n, ++cur)
			{
				construct(&*cur, x);
			}
			return cur;
		}
		catch (...)
		{
			destory(first, cur);
			throw;
		}
	}

	template<typename ForwardIterator, typename Size, typename T,typename T1>
	inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
	{
		typedef typename __type_traits<T1>::is_POD_type is_POD;
		return __uninitialized_fill_n_aux(first, n, x, is_POD());
	}

	template<typename ForwardIterator, typename Size, typename T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
	{
		return __uninitialized_fill_n(first, n, x, value_type(first));
	}

	//uniitialized_copy
	template<typename InputIterator, typename ForwardIterator>
	inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type)
	{
		return std::copy(first, last, result);
	}

	template<typename InputIterator, typename ForwardIterator>
	inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type)
	{
		ForwardIterator cur = result;
		try
		{
			for (; first != last; ++first, ++cur)
			{
				construct(&*cur, *first);
			}
			return cur;
		}
		catch (...)
		{
			destory(result, cur);
			throw;
		}
	}

	template<typename InputIterator, typename ForwardIterator, typename T>
	inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*)
	{
		typedef typename __type_traits<T>::is_POD_type is_POD;
		return __uninitialized_copy_aux(first, last, result, is_POD());
	}

	template<typename InputIterator,typename ForwardIterator>
	inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
	{
		return __uninitialized_copy(first, last, result, value_type(result));
	}

	template<>
	inline char* uninitialized_copy(const char* first, const char* last, char* result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	template<>
	inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
	{
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

	//uninitialized_fill
	template<typename ForwardIterator, typename T>
	inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type)
	{
		std::fill(first, last, x);
	}

	template<typename ForwardIterator, typename T>
	void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type)
	{
		ForwardIterator cur = first;
		try
		{
			for (; cur != last; ++cur)
			{
				construct(&*cur, x);
			}
		}
		catch (...)
		{
			destory(first, cur);
			throw;
		}
	}

	template<typename ForwardIterator, typename T, typename T1>
	inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*)
	{
		typedef typename __type_traits<T1>::is_POD_type is_POD;
		__uninitialized_fill_aux(first, last, x, is_POD());
	}

	template<typename ForwardIterator, typename T>
	inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
	{
		__uninitialized_fill(first, last, x, value_type(first));
	}
}


#endif
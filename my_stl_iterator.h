#ifndef _MY_STL_ITERATOR_H
#define _MY_STL_ITERATOR_H

#include<cstddef>

namespace mySTL
{
	struct input_iterator_tag{};
	struct output_iterator_tag{};
	struct forward_iterator_tag :public input_iterator_tag{};
	struct bidirectional_iterator_tag : public forward_iterator_tag{};
	struct random_access_iterator_tag : public bidirectional_iterator_tag{};

	//自行开发的迭代器最好继承自下面这个struct
	template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*, typename Reference = T&>
	struct iterator
	{
		typedef Category iterator_category;
		typedef T value_type;
		typedef Distance difference_type;
		typedef Pointer pointer;
		typedef Reference reference;
	};

	//traits
	template<typename Iterator>
	struct iterator_traits
	{
		typedef typename Iterator::iterator_category iterator_category;
		typedef typename Iterator::value_type value_type;
		typedef typename Iterator::difference_type difference_type;
		typedef typename Iterator::pointer poiter;
		typedef typename Iterator::reference reference;
	};

	//原生指针的特化版本
	template<typename T>
	struct iterator_traits<T*>
	{
		typedef random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef T& reference;
	};

	//原生指针底层const的特化版
	template<typename T>
	struct iterator_traits<const T*>
	{
		typedef random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef T& reference;
	};

	//迭代器类型
	template<typename Iterator>
	inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&)
	{
		typedef typename iterator_traits<Iterator>::iterator_category category;
		return category();
	}

	//迭代器difference type
	template<typename Iterator>
	inline typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}

	//迭代器value type
	template<typename Iterator>
	inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

	//distance函数
	template<typename InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type __distance(InputIterator first, InputIterator last, input_iterator_tag)
	{
		iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last)
		{
			++first;
			++n;
		}
		return n;
	}

	template<typename InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type __distance(InputIterator first, InputIterator last, random_access_iterator_tag)
	{
		return last - first;
	}

	template<typename InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last)
	{
		typedef typename iterator_traits<InputIterator>::iterator_category category;
		return __distance(first, last, category());
	}

	//advance函数
	template<typename InputIterator,typename Distance>
	inline void __advance(InputIterator& i, Distance n, input_iterator_tag)
	{
		while (n--)
		{
			++i;
		}
	}

	template<typename BidirectionalIterator, typename Distance>
	inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
	{
		if (n >= 0)
		{
			while (n--)
			{
				++i;
			}
		}
		else
		{
			while(n++)
			{
				--i;
			}
		}
	}

	template<typename RandomAccessIterator, typename Distance>
	inline void __advance(RandomAccessIterator&i, Distance n, random_access_iterator_tag)
	{
		i += n;
	}

	template<typename InputIterator,typename Distance>
	inline void advance(InputIterator& i, Distance n)
	{
		__advance(i, n, iterator_category(i));
	}

}

#endif
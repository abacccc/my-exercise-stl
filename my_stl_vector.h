#ifndef _MY_STL_VECTOR
#define _MY_STL_VECTOR

#include"my_memory.h"
#include"my_stl_uninitialized.h"
#include"my_stl_algorithm.h"

//�ж��Ƿ����my_stl_algorithm.h
#ifndef _MY_STL_ALGORITHM_H
#	include<algorithm>
#endif

namespace mySTL
{
	/*
	Ĭ��ʹ�õڶ���������
	*/
	template<typename T, typename Alloc = alloc>
	class my_vector
	{
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef value_type* iterator;
		typedef value_type& reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

	/*
	private��ֻ����1.�����еĺ�����2.����Ԫ�������ʡ�
	���ܱ��κ��������ʣ�����Ķ���Ҳ���ܷ��ʡ�

	protected�����Ա�1.�����еĺ�����2.����ĺ������Լ�3.����Ԫ�������ʡ�
	�����ܱ�����Ķ�����ʡ�

	public�����Ա�1.�����еĺ�����2.����ĺ�����3.����Ԫ�������ʣ�Ҳ������4.����Ķ�����ʡ�
	*/
	protected:
		typedef simple_alloc<value_type, Alloc> data_allocator;
		iterator start;                  //��ʹ�ÿռ��ͷ
		iterator finish;                 //��ʹ�ÿռ��β
		iterator end_of_storage;         //���ÿռ��β

		void insert_aux(iterator position, const T& x)
		{
			if (finish != end_of_storage)
			{
				construct(finish, *(finish - 1));
				++finish;
				T x_copy = x;
				std::copy_backward(position, finish - 2, finish - 1);
				*position = x_copy;
			}
			else
			{
				const size_type old_size = size();
				const size_type len = old_size != 0 ? 2 * old_size : 1;
				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;
				try
				{
					new_finish = uninitialized_copy(start, position, new_start);
					construct(new_finish, x);
					++new_finish;
					new_finish = uninitialized_copy(position, finish, new_finish);
				}
				catch (...)
				{
					destory(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}
				destory(begin(), end());
				deallocate();

				start = new_start;
				finish = new_finish;
				end_of_storage = start + len;
			}
		}

		void deallocate()
		{
			if (start)
			{
				data_allocator::deallocate(start, end_of_storage - start);
			}
		}

		//���ÿռ���������
		iterator allocate_and_fill(size_type n, const T& x)
		{
			iterator result = data_allocator::allocate(n);
			uninitialized_fill_n(result, n, x);
			return result;
		}

		void fill_initialize(size_type n, const T& value)
		{
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
		}

	public:
		
		//���캯��
		my_vector() :start(0), finish(0), end_of_storage(0){}
		my_vector(size_type n, const T& value)
		{
			fill_initialize(n, value);
		}
		my_vector(int n, const T& value)
		{
			fill_initialize(n, value);
		}
		my_vector(long n, const T& value)
		{
			fill_initialize(n, value);
		}
		//��ֹ��ʽ����ת��
		explicit my_vector(size_type n)
		{
			fill_initialize(n, T());
		}

		//��������
		~my_vector()
		{
			destory(start, finish);
			deallocate();
		}

		//�ӿ�
		iterator begin() const
		{
			return start;
		}

		iterator end() const
		{
			return finish;
		}

		size_type size() const
		{
			return size_type(end() - begin());
		}

		size_type capacity() const
		{
			return size_type(end_of_storage - begin());
		}

		bool empty() const
		{
			return begin() == end();
		}

		reference operator[](size_type n)
		{
			return *(begin() + n);
		}

		reference front()
		{
			return *begin();
		}

		reference back()
		{
			return *(end() - 1);
		}


		//�ı�Ԫ�صĲ���
		void push_back(const T& x)
		{
			if (finish != end_of_storage)
			{
				construct(finish, x);
				++finish;
			}
			else
			{
				insert_aux(end(), x);
			}
		}

		void pop_back()
		{
			--finish;
			destory(finish);
		}

		iterator erase(iterator position)
		{
			if (position + 1 != end())
			{
				std::copy(position + 1, finish, position);
			}
			--finish;
			destory(finish);
			return position;
		}

		iterator erase(iterator first, iterator last)
		{
			iterator i = std::copy(last, finish, first);
			destory(i, finish);
			finish = finish - (last - first);
			return first;
		}

		void clear()
		{
			erase(begin(), end());
		}

		void insert(iterator position, size_type n, const T& x)
		{
			if (n != 0)
			{
				if (size_type(end_of_storage - finish) >= n)
				{
					T x_copy = x;
					const size_type elems_after = finish - position;
					iterator old_finish = finish;
					if (elems_after > n)
					{
						uninitialized_copy(finish - n, finish, finish);
						finish += n;
						std::copy_backward(position, old_finish - n, old_finish);
						std::fill(position, position, x_copy);
					}
					else
					{
						uninitialized_fill_n(finish, n - elems_after, x_copy);
						finish += n - elems_after;
						uninitialized_copy(position, old_finish, finish);
						finish += elems_after;
						std::fill(position, old_finish, x_copy);
					}
				}
				else
				{
					const size_type old_size = size();
					const size_type len = old_size + std::max(old_size, n);
					iterator new_start = data_allocator::allocate(len);
					iterator new_finish = new_start;
					try
					{
						new_finish = uninitialized_copy(start, position, new_start);
						new_finish = uninitialized_fill_n(new_finish, n, x);
						new_finish = uninitialized_copy(position, finish, new_finish);
					}
					catch (...)
					{
						destory(new_start, new_finish);
						data_allocator::deallocate(new_start, len);
						throw;
					}
					destory(start, finish);
					deallocate();

					start = new_start;
					finish = new_finish;
					end_of_storage = new_start + len;
				}
				
			}
		}

		void resize(size_type new_size, const T& x)
		{
			if (new_size < size())
			{
				erase(begin() + new_size, end());
			}
			else
			{
				insert(end(), new_size - size(), x);
			}
		}
	};
}


#endif
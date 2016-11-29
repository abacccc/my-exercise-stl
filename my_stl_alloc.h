#ifndef _MY_STL_ALLOC
#define _MY_STL_ALLOC

#if 0
# inlcude<new>
# define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
# include<iostream>
# define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)
#endif

namespace mySTL
{
	//��һ��������
	template<int inst>
	class __malloc_alloc_template
	{
	private:
		//���������������������ڴ治��������oomΪout of memory
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		static void(*__malloc_alloc_oom_handler)();

	public:
		static void* allocate(size_t n)
		{
			void* result = malloc(n);
			if (result == 0)
			{
				result = oom_malloc(n);
			}
			return result;
		}

		static void deallocate(void* p, size_t)
		{
			free(p);
			/*void free (void* ptr);
			A block of memory previously allocated by a call to malloc, calloc or realloc is deallocated, making it available again for further allocations.
			��ֱ���ͷ�a block of memory
			*/
		}

		static void* reallocate(void* p, size_t/*ԭʼ��С*/, size_t new_sz)
		{
			void* result = realloc(p, new_sz);
			if (result == 0)
			{
				result = oom_realloc(p, new_sz);
			}
			return result;
		}

		//����set_new_handler
		static void(*set_malloc_handler(void(*f)()))()
		{
			void(*old)() = __malloc_alloc_oom_handler;
			__malloc_alloc_oom_handler = f;
			return old;
			/*
			�˺�������һ������ָ�룬�൱��
			auto set_malloc_handler(void(*f)()) -> void(*)()
			*/
		}
		
	};

	//out of memory��handler��ֵ��Ϊ0����Ҫ�û��Լ��趨
	template<int inst>
	void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

	template<int inst>
	void* __malloc_alloc_template<inst>::oom_malloc(size_t n)
	{
		void(*my_malloc_handler)();
		void* result;

		for (;;)//���ϳ��������ڴ�
		{
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (__malloc_alloc_oom_handler == 0)
			{
				__THROW_BAD_ALLOC;
			}
			(*my_malloc_handler)();//����handler����
			result = malloc(n);
			if (result)
			{
				return result;
			}
		}
	}

	template<int inst>
	void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n)
	{
		void(*my_malloc_handler)();
		void* result;

		for (;;)//���ϳ��������ڴ�
		{
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (__malloc_alloc_oom_handler == 0)
			{
				__THROW_BAD_ALLOC;
			}
			(*my_malloc_handler)();//����handler����
			result = realloc(p, n);
			if (result)
			{
				return result;
			}
		}
	}

	typedef __malloc_alloc_template<0> malloc_alloc;

	//alloc�ķ���STL��׼�Ľӿ�
	template<typename T,typename Alloc>
	class simple_alloc
	{
	public:
		static T* allocate(size_t n)
		{
			return 0 == n ? 0 : (T*)Alloc::allocate(n*sizeof(T));
		}
		static T* allocate(void)
		{
			return (T*)Alloc::allocate(sizeof(T));
		}
		static void deallocate(T* p, size_t n)
		{
			if (0 != n)
			{
				Alloc::deallocate(p, n*sizeof(T));
			}
		}
		static void deallocate(T* p)
		{
			Alloc::deallocate(p, sizeof(T));
		}
	};


	//�ڶ���������
	enum{__ALIGN = 8};//С�����ϵ��߽�
	enum{__MAX_BYTE = 128};//С�����Ͻ�
	enum{__NFREELISTS = __MAX_BYTE / __ALIGN};//free list����

	template<bool threads,int inst>
	class __default_alloc_templete
	{
	private:
		static size_t ROUND_UP(size_t bytes)
		{
			return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
		}
	private:
		union obj
		{
			union obj* free_list_link;
			char client_data[1];
		};
	private:
		/*
		volatile�ؼ��ֿ����������ѱ�����������������ı�����ʱ�п��ܸı䣬��˱����ĳ���ÿ����Ҫ�洢���ȡ���������ʱ�򣬶���ֱ�Ӵӱ�����ַ�ж�ȡ���ݡ�
		���û��volatile�ؼ��֣�������������Ż���ȡ�ʹ洢��������ʱʹ�üĴ����е�ֵ�������������ɱ�ĳ�������˵Ļ��������ֲ�һ�µ�����
		*/
		static obj* volatile free_list[__NFREELISTS];
		
		//���������С����ʹ���Ǹ�free ist����1��ʼ����
		static size_t FREELIST_INDEX(size_t bytes)
		{
			return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
		}

		static void* refill(size_t n);
		static char* chunk_alloc(size_t size, int& nobjs);

		static char* start_free;
		static char* end_free;
		static size_t heap_size;

	public:

		static void* allocate(size_t n)
		{
			/*
			�������ԭ������ʽ�ǣ�obj** my_free_list�������Ļ�*my_free_list�����е��ڴ��ָ�������е�һ��Ԫ�أ����ܱ��Ż����Ĵ����У�
			�Ӷ�ʹ������޷�lockס�����Ķ����ã�����ڼĴ���������һ���߳̿��ܻ��������޸ĸüĴ�����ֵ�������ڴ���������һ���߳�û�з���Ȩ�����Բ����޸ģ���
			Ҫ���������������ڴ��о�Ҫ��volatile���Σ��������ε���*my_free_list����free_list�����е�һ��Ԫ�أ�����������ָ�룬����volatile��������*�м䡣
			*/
			obj * volatile * my_free_list;
			
			obj* result;
			if (n > (size_t)__MAX_BYTE)
			{
				return malloc_alloc::allocate(n);
			}

			//Ѱ�Һ��ʵ�free list
			my_free_list = free_list + FREELIST_INDEX(n);
			result = *my_free_list;
			if (result == 0)
			{
				//û�ҵ����������free list
				void* r = refill(ROUND_UP(n));
				return r;
			}
			//����free list
			*my_free_list = result->free_list_link;
			return result;
		}

		static void deallocate(void* p, size_t n)
		{
			obj* q = (obj*)p;
			obj * volatile * my_free_list;

			if (n > (size_t)__MAX_BYTE)
			{
				malloc_alloc::deallocate(p, n);
				return;
			}

			my_free_list = free_list + FREELIST_INDEX(n);
			q->free_list_link = *my_free_list;
			*my_free_list = q;
		}

		static void* reallocate(void* p, size_t old_sz, size_t new_sz)
		{
			void* result;
			size_t copy_sz;

			if (old_sz > (size_t)__MAX_BYTE && new_sz > (size_t)__MAX_BYTE)
			{
				return realloc(p, new_sz);
			}

			if (ROUND_UP(old_sz) == ROUND_UP(new_sz))
			{
				return p;
			}

			result = allocate(new_sz);
			copy_sz = new_sz > old_sz ? old_sz : new_sz;
			memcpy(result, p, copy_sz);
			deallocate(p, old_sz);
			return (result);
		}

	};

	//����Ϊ��ֵ�趨
	template<bool threads,int inst>
	char* __default_alloc_templete<threads, inst>::start_free = 0;

	template<bool threads, int inst>
	char* __default_alloc_templete<threads, inst>::end_free = 0;

	template<bool threads, int inst>
	size_t __default_alloc_templete<threads, inst>::heap_size = 0;

	template<bool threads,int inst>
	typename __default_alloc_templete<threads, inst>::obj* volatile __default_alloc_templete<threads, inst>::free_list[__NFREELISTS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	template<bool threads,int inst>
	void* __default_alloc_templete<threads, inst>::refill(size_t n)
	{
		int nobjs = 20; //ȱʡȡ��20�������飨�ڵ㣩
		char* chunk = chunk_alloc(n, nobjs); //nobjsΪ�����õ���
		obj* volatile* my_free_list;
		obj* result;
		obj* current_obj, *next_obj;
		int i;

		//���ֻ���뵽1���ڵ㣬ֱ�Ӹ�������ʹ�ã�free list���½ڵ�
		if (1 == nobjs)
		{
			return chunk;
		}

		//����free list�����½ڵ�
		my_free_list = free_list + FREELIST_INDEX(n);

		//ʣ��ڵ����ӵ�free list��Ӧ��С�Ľڵ���
		result = (obj*)chunk;
		*my_free_list = next_obj = (obj*)(chunk + n);
		for (i = 1;; ++i)
		{
			current_obj = next_obj;
			next_obj = (obj*)((char*)next_obj + n);
			if (nobjs - 1 == i)
			{
				current_obj->free_list_link = 0;
				break;
			}
			else
			{
				current_obj->free_list_link = next_obj;
			}
		}
		return(result);
	}

	template<bool threads,int inst>
	char* __default_alloc_templete<threads, inst>::chunk_alloc(size_t size, int& nobjs)
	{
		char* result;
		size_t total_bytes = size * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes)
		{
			result = start_free;
			start_free += total_bytes;
			return (result);
		}
		else if (bytes_left >= size)
		{
			nobjs = bytes_left / size;
			total_bytes = size * nobjs;
			result = start_free;
			start_free += total_bytes;
			return (result);
		}
		else
		{
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);//???????????????????
			//��������ڴ��
			if (bytes_left > 0)
			{
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj*)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj*)start_free;
			}

			//��heap�����ڴ��
			start_free = (char*)malloc(bytes_to_get);
			if (0 == start_free)
			{
				int i;
				obj* volatile* my_free_list, *p;

				for (i = size; i <= __MAX_BYTE; i += __ALIGN)
				{
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (0 != p)
					{
						*my_free_list = p->free_list_link;
						start_free = (char*)p;
						end_free = start_free + i;
						return (chunk_alloc(size, nobjs));
					}
				}
				end_free = 0;
				start_free = (char*)malloc_alloc::allocate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return (chunk_alloc(size, nobjs));
		}		
	}

	
	//����ڶ���������ΪĬ��������
	typedef __default_alloc_templete<0, 0> alloc;
}

#endif
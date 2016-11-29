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
	//第一级配置器
	template<int inst>
	class __malloc_alloc_template
	{
	private:
		//以下三个函数用来处理内存不足的情况，oom为out of memory
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
			可直接释放a block of memory
			*/
		}

		static void* reallocate(void* p, size_t/*原始大小*/, size_t new_sz)
		{
			void* result = realloc(p, new_sz);
			if (result == 0)
			{
				result = oom_realloc(p, new_sz);
			}
			return result;
		}

		//仿真set_new_handler
		static void(*set_malloc_handler(void(*f)()))()
		{
			void(*old)() = __malloc_alloc_oom_handler;
			__malloc_alloc_oom_handler = f;
			return old;
			/*
			此函数返回一个函数指针，相当于
			auto set_malloc_handler(void(*f)()) -> void(*)()
			*/
		}
		
	};

	//out of memory的handler初值设为0，需要用户自己设定
	template<int inst>
	void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

	template<int inst>
	void* __malloc_alloc_template<inst>::oom_malloc(size_t n)
	{
		void(*my_malloc_handler)();
		void* result;

		for (;;)//不断尝试配置内存
		{
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (__malloc_alloc_oom_handler == 0)
			{
				__THROW_BAD_ALLOC;
			}
			(*my_malloc_handler)();//运行handler函数
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

		for (;;)//不断尝试配置内存
		{
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (__malloc_alloc_oom_handler == 0)
			{
				__THROW_BAD_ALLOC;
			}
			(*my_malloc_handler)();//运行handler函数
			result = realloc(p, n);
			if (result)
			{
				return result;
			}
		}
	}

	typedef __malloc_alloc_template<0> malloc_alloc;

	//alloc的符合STL标准的接口
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


	//第二级配置器
	enum{__ALIGN = 8};//小区块上调边界
	enum{__MAX_BYTE = 128};//小区块上界
	enum{__NFREELISTS = __MAX_BYTE / __ALIGN};//free list个数

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
		volatile关键字可以用来提醒编译器它后面所定义的变量随时有可能改变，因此编译后的程序每次需要存储或读取这个变量的时候，都会直接从变量地址中读取数据。
		如果没有volatile关键字，则编译器可能优化读取和存储，可能暂时使用寄存器中的值，如果这个变量由别的程序更新了的话，将出现不一致的现象。
		*/
		static obj* volatile free_list[__NFREELISTS];
		
		//根据区块大小决定使用那个free ist，从1开始算起
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
			这个声明原来的形式是：obj** my_free_list，这样的话*my_free_list（空闲的内存块指针数组中的一个元素）可能被优化到寄存器中，
			从而使库代码无法lock住对它的读调用（如果在寄存器中则另一个线程可能会无意中修改该寄存器的值，而在内存中由于另一个线程没有访问权力所以不能修改）。
			要声明变量必须在内存中就要用volatile修饰，这里修饰的是*my_free_list，是free_list数组中的一个元素，而不是数组指针，所以volatile放在两个*中间。
			*/
			obj * volatile * my_free_list;
			
			obj* result;
			if (n > (size_t)__MAX_BYTE)
			{
				return malloc_alloc::allocate(n);
			}

			//寻找合适的free list
			my_free_list = free_list + FREELIST_INDEX(n);
			result = *my_free_list;
			if (result == 0)
			{
				//没找到，重新填充free list
				void* r = refill(ROUND_UP(n));
				return r;
			}
			//调整free list
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

	//以下为初值设定
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
		int nobjs = 20; //缺省取得20个新区块（节点）
		char* chunk = chunk_alloc(n, nobjs); //nobjs为传引用调用
		obj* volatile* my_free_list;
		obj* result;
		obj* current_obj, *next_obj;
		int i;

		//如果只申请到1个节点，直接给调用者使用，free list无新节点
		if (1 == nobjs)
		{
			return chunk;
		}

		//调整free list纳入新节点
		my_free_list = free_list + FREELIST_INDEX(n);

		//剩余节点连接到free list对应大小的节点上
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
			//充分利用内存池
			if (bytes_left > 0)
			{
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj*)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj*)start_free;
			}

			//用heap补充内存池
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

	
	//定义第二级配置器为默认配置器
	typedef __default_alloc_templete<0, 0> alloc;
}

#endif
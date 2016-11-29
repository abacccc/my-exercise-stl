#ifndef _MY_TYPE_TRAITS
#define _MY_TYPE_TRAITS

namespace mySTL
{
	struct __true_type{};
	struct __false_type{};

	template<typename type>
	struct __type_traits
	{
		typedef __true_type this_dummy_member_be_first;  //通知编译器此__type_traits结构体使特殊的

		typedef __false_type has_trivial_default_constructor;
		typedef __false_type has_trivial_copy_constructor;
		typedef __false_type has_trivial_assignment_operator;
		typedef __false_type has_trivial_destructor;

		/*
		Specifies that the type is POD (Plain Old Data) type. This means the type is compatible with the types used in the C programming language, 
		can be manipulated using C library functions: it can be created with std::malloc, 
		it can be copied with std::memmove, etc, and can be exchanged with C libraries directly, in its binary form.
		*/
		typedef __false_type is_POD_type;

		/*
		SGI定义的为最保守的值，所以全为__false_type
		*/
	};

	//一下为各类型的__type_traits类的显示特化
	template<>
	struct __type_traits<char>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<signed char>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<unsigned char>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<wchar_t>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<short>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<unsigned short>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<int>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<unsigned int>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<long>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<unsigned long>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<long long>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<unsigned long long>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<float>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<double>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};

	template<>
	struct __type_traits<long double>
	{
		typedef __true_type has_trivial_default_constructor;
		typedef __true_type has_trivial_copy_constructor;
		typedef __true_type has_trivial_assignment_operator;
		typedef __true_type has_trivial_destructor;
		typedef __true_type is_POD_type;
	};
}


#endif
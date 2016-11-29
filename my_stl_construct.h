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
		placement new��ֻ��operator new���ص�һ���汾�������������ڴ棬ֻ�Ƿ���ָ���Ѿ�����õ�ĳ���ڴ��һ��ָ�롣��˲���ɾ����������Ҫ���ö��������������
		�ںܶ�����£�placement new��ʹ�÷�����������ͨ��new������ͬ�������ṩ������ʹ�ò��衣

		��һ��  ������ǰ����

		�����ַ�ʽ��

		1.Ϊ�˱�֤ͨ��placement newʹ�õĻ�������memory alignment(�ڴ����)��ȷ׼����ʹ����ͨ��new�����������ڶ��Ͻ��з���
		class Task ;
		char * buff = new [sizeof(Task)]; //�����ڴ�
		(��ע��auto����static�ڴ沢�Ƕ���ȷ��Ϊÿһ�������������У����ԣ��㽫������placement newʹ�����ǡ�)

		2.��ջ�Ͻ��з���
		class Task ;
		char buf[N*sizeof(Task)]; //�����ڴ�

		3.����һ�ַ�ʽ������ֱ��ͨ����ַ��ʹ�á�(������������ĵ�ַ)
		void* buf = reinterpret_cast<void*> (0xF00F);

		�ڶ���������ķ���

		�ڸղ��ѷ���Ļ���������placement new������һ������
		Task *ptask = new (buf) Task

		��������ʹ��

		������ͨ��ʽʹ�÷���Ķ���

		ptask->memberfunction();

		ptask-> member;

		//...

		���Ĳ������������

		һ����ʹ�������������������������������������������������ķ�ʽ��������������
		ptask->~Task(); //�������ڵ���������

		���岽���ͷ�

		����Է������û��沢��������һ���µĶ����ظ�����2��3��4������㲻�����ٴ�ʹ��������棬������������ͷ�����delete [] buf;

		�����κβ���Ϳ��ܵ�������ʱ��ı������ڴ�й¶���Լ����������벻��������������ȷʵ��Ҫʹ��placement new����������ѭ���ϵĲ��衣
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
		__destory_aux(first, last, trivial_destructor());  //�ж��Ƿ���trivial destructor
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
#pragma once

#ifdef USE_FASTL

namespace fastl 
{ 
	template<typename T> class vector
	{ 
	public:
		typedef T        value_type;
		typedef size_t   size_type;

		typedef T*       iterator;
		typedef const T* const_iterator;
		typedef T&       reference;
		typedef const T& const_reference;

	public:
		vector();
		explicit vector(size_t size);
		~vector();

		//TODO ~ ramonv ~ copy constructor
		//TODO ~ ramonv ~ move constructor

		//vector<T>& operator=(const vector<T>& arg); 
		//TODO ~ ramonv ~ move assignment

		inline reference operator[](size_type index) { return m_data[index]; }
    inline const_reference operator[](size_type index) const { return m_data[index]; }

		inline size_type size() const{ return m_size; }
		inline size_type capacity() const { return m_capacity; }
		
		inline iterator begin() { return m_data; }
		inline const_iterator begin() const { return m_data; }
		inline iterator end() { return m_data+m_size;	} 
		inline const_iterator end() const { return m_data+m_size;	}
		inline reference back() { return m_data[m_size-1]; }
		inline bool empty() const { return m_size == 0u; }

		void reserve(const size_type size);
		void resize(const size_type size);
		void clear();

		//void push_back(value_type&& value);
		void push_back(const value_type& value);
		void pop_back();

		void insert(iterator it,const value_type& value);
		//void insert(iterator it,const value_type&& value);

		iterator erase(iterator it);
		//iterator erase(iterator firstArg,iterator secondArg); 

		private:
			value_type*	m_data;
			size_type	  m_size;
			size_type   m_capacity;
	};

	//Implementation

	//------------------------------------------------------------------------------------------
	template<typename T> vector<T>::vector()
		: m_data(nullptr)
		, m_size(0u)
		, m_capacity(0u)
	{
	}

	//------------------------------------------------------------------------------------------
	template<typename T> vector<T>::vector(size_t size)
		: m_data(new T[size])
		, m_size(0u)
		, m_capacity(size)
	{ 
	}

	//------------------------------------------------------------------------------------------
	template<typename T> vector<T>::~vector() 
	{ 
		delete[] m_data; 
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline void vector<T>::reserve(const size_type size)
	{
		if (size > m_capacity)
		{ 
			T* newData = new T[size];

			for (int i = 0; i < m_size; ++i)
			{
				newData[i] = m_data[i];
			}

			delete[] m_data;
			m_data = newData;
			m_capacity = size;
		}
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline void vector<T>::resize(const size_type size)
	{
		reserve(size);

		for (size_type i=size;i<m_size;++i)
		{
			m_data[i].~T();
		}

		for (size_type i=m_size;i<size;++i)
		{
			m_data[i] = T();
		}			

		m_size = size;
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline void vector<T>::clear()
	{
		resize(0u);
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline void vector<T>::push_back(const value_type& value)
	{
 		if (m_size == m_capacity)
		{ 
			reserve(m_capacity == 0u? 8u : 2u*m_capacity);
		}

		m_data[m_size++] = value;
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline void vector<T>::pop_back()
	{ 
		if (!empty())
		{ 
			resize(m_size-1);
		}
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline void vector<T>::insert(iterator it,const value_type& value)
	{ 
		if (m_size == m_capacity)
		{ 
			reserve(m_capacity == 0u? 8u : 2u*m_capacity);
		}

		for (iterator i = end(); i > it;--i)
		{
			*i = *(i-1);
		}

		*it = value;
		++m_size;
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline typename vector<T>::iterator vector<T>::erase(iterator it)
	{ 
		it->~T();

		const_iterator end = end();
		for (iterator i = it; i < end; ++i)
		{
			*i = *(i+1);
		}

		--m_size;
	}

}

#else

#include <vector>

namespace fastl
{ 
	template<typename T> using vector = std::vector<T>;
}

#endif
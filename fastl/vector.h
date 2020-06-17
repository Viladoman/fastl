#pragma once

#ifdef USE_FASTL

//Strangely the model std::vector I took for reference uses the assignment operator for moving elements on erase
//#define FASTL_VECTOR_ERASE_WITH_ASSIGNMENT

//Forward declare the placement new in order to avoid #include <new> 
void* operator new  (size_t size, void* ptr) noexcept; 

namespace fastl 
{ 
	//------------------------------------------------------------------------------------------
	//Consider moving this around if needed somewhere else 
	template <class T> struct remove_reference { typedef T type; };
	template <class T> struct remove_reference<T&> { typedef T type; };
	template <class T> struct remove_reference<T&&> { typedef T type; };
	template <typename T> typename remove_reference<T>::type&& move(T&& arg) { return static_cast<typename remove_reference<T>::type&&>(arg);	}

	template<typename T, typename ... Args> void Construct(T* ptr, Args&&... args) { new (ptr) T(move(args)...); }

	////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T> class vector
	{ 
	private: 
		enum { DEFAULT_CAPACITY_SIZE = 8 };
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
		vector(const vector<T>& input);

		//TODO ~ ramonv ~ We can fake initializer list with an enable_if SFINAE and a variadic template

		~vector();

		vector<T>& operator = (const vector<T>& t);

		reference operator[](size_type index) { return m_data[index]; }
		const_reference operator[](size_type index) const { return m_data[index]; }

		size_type size() const{ return m_size; }
		size_type capacity() const { return m_capacity; }
		
		iterator begin() { return m_data; }
		const_iterator begin() const { return m_data; }
		iterator end() { return m_data+m_size;	} 
		const_iterator end() const { return m_data+m_size;	}
		reference back() { return m_data[m_size-1]; }
		bool empty() const { return m_size == 0u; }

		void reserve(const size_type size);
		void resize(const size_type size);
		void clear();

		void push_back(const value_type& value);
		void insert(iterator it,const value_type& value);
		template<typename ... Args> void emplace(iterator it, Args&&... args);
		template<typename ... Args> void emplace_back(Args&&... args);

		void pop_back();


		iterator erase(iterator it);
		iterator erase(iterator fromIt,iterator toIt); 

		private:
			value_type* m_data;
			size_type   m_size;
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
		, m_size(size)
		, m_capacity(size)
	{ 
	}

	//------------------------------------------------------------------------------------------
	template<typename T> vector<T>::vector(const vector<T>& input)
		: m_data(new T[input.m_size])
		, m_size(input.m_size)
		, m_capacity(input.m_capacity)
	{
		for (size_t i = 0u; i < m_size; ++i)
		{
			m_data[i] = input[i];
		} 
	}

	//------------------------------------------------------------------------------------------
	template<typename T> vector<T>::~vector() 
	{ 
		for (size_type i=0u;i<m_size;++i)
		{
			m_data[i].~T();
		}
		
		delete[] reinterpret_cast<char*>(m_data);
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline vector<T>& vector<T>::operator= (const vector<T>& input)
	{
		clear();
		reserve(input.m_capacity);
		m_size = input.m_size;
		for (size_type i = 0u; i < m_size; ++i)
		{
			Construct<T>(&m_data[i], input[i]);
		}
		return *this;
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline void vector<T>::reserve(const size_type size)
	{
		if (size > m_capacity)
		{ 
			m_capacity = size;
			T* newData = reinterpret_cast<T*>(new char[m_capacity*sizeof(T)]);

			for (size_type i = 0u; i < m_size; ++i)
			{
				Construct<T>(&newData[i],m_data[i]);
				m_data[i].~T();
			}

			delete[] reinterpret_cast<char*>(m_data);
			m_data = newData;
			
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
			Construct<T>(&m_data[i]);
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
		return emplace(end(), value);
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline void vector<T>::insert(iterator it,const value_type& value)
	{ 
		emplace(it, value);
	}

	//------------------------------------------------------------------------------------------
	template<typename T> template<typename ... Args> void vector<T>::emplace_back(Args&&... args)
	{
		emplace(end(),move(args)...);
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
	template<typename T> template<typename ... Args> void vector<T>::emplace(iterator it, Args&&... args)
	{ 
		const size_type index = it-begin();

		if (m_size == m_capacity)
		{ 
			reserve(m_capacity == 0u? DEFAULT_CAPACITY_SIZE : 2u*m_capacity);
		}

		iterator insertIt = begin() + index;
		for (iterator i = end(); i > insertIt;--i)
		{
			i->~T(); 
			Construct<T>(i,*(i-1));
		}

		Construct<T>(insertIt,move(args)...);
		++m_size;
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline typename vector<T>::iterator vector<T>::erase(iterator it)
	{ 
		return erase(it,it+1);
	}

	//------------------------------------------------------------------------------------------
	template<typename T> inline typename vector<T>::iterator vector<T>::erase(iterator fromIt, iterator toIt)
	{
		const size_type rangeSize = toIt-fromIt;
		const_iterator batchEndIt = end()-rangeSize;

		for (iterator i = fromIt; i < batchEndIt; ++i)
		{
#ifdef FASTL_VECTOR_ERASE_WITH_ASSIGNMENT
			* i = *(i + rangeSize);
#else 
			i->~T(); 
			Construct<T>(i,*(i+rangeSize));
#endif
		}

		resize(m_size - rangeSize);
		return fromIt;
	}
}

#else

#include <vector>

namespace fastl
{ 
	template<typename T> using vector = std::vector<T>;
}

#endif //USE_FASTL


#ifdef FASTL_EXPOSE_PLAIN_ALIAS

template<typename T> using vector = fastl::vector<T>;

#endif //FASTL_EXPOSE_PLAIN_ALIAS
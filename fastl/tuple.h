#pragma once

#ifdef USE_FASTL

namespace fastl
{
	namespace TupleImpl
	{ 
		template<int ...> struct Sequence { };

		template<int N,int ...S> struct SequenceGenerator: SequenceGenerator<N-1,N-1,S...> { }; // General case - Recursion
		template<int ...S> struct SequenceGenerator<0,S...> { typedef Sequence<S...> type; };   // Base case    - Last element

		template<size_t index,typename ... Ts> struct ListAt;
		template<typename T0,typename ... Ts> struct ListAt<0,T0,Ts...> { typedef T0 type; };
		template<size_t index,typename T0,typename ... Ts> struct ListAt<index,T0,Ts...> { typedef typename ListAt<index-1,Ts...>::type type; };
	}

	// Definition
	template <size_t index,typename T> struct tupleLeaf { T value; };

	template<typename Sequence,typename... Ts> struct tupleImpl;
	template <size_t... I,typename... Ts> struct tupleImpl<TupleImpl::Sequence<I...>,Ts...>: public tupleLeaf<I,Ts>... { };

	template<typename ... Ts> struct tuple { tupleImpl<typename TupleImpl::SequenceGenerator<sizeof...(Ts)>::type,Ts...> impl; };

	// Methods
	template<size_t index,typename T,typename ... Ts> inline T&       tupleGetImpl(tuple<Ts...>& input)       { return static_cast<tupleLeaf<index,T>&>(input.impl).value; }
	template<size_t index,typename T,typename ... Ts> inline const T& tupleGetImpl(const tuple<Ts...>& input) { return static_cast<const tupleLeaf<index,T>&>(input.impl).value; }

	template<size_t index,typename Tuple> struct tuple_element;
	template<size_t index,typename ... Ts> struct tuple_element<index,tuple<Ts...> > { typedef typename TupleImpl::ListAt<index,Ts...>::type type; };

	template<size_t index,typename ... Ts> inline auto& get(tuple<Ts...>& input) { return tupleGetImpl<index,typename tuple_element<index,tuple<Ts...>>::type>(input); }
	template<size_t index,typename ... Ts> inline auto& get(const tuple<Ts...>& input) { return tupleGetImpl<index,typename tuple_element<index,tuple<Ts...>>::type>(input); }
}

#else 

namespace fastl
{
	template< typename ... Ts > using tuple = std::tuple<Ts...>;

	template<size_t index,typename Tuple> struct tuple_element{ typedef typename std::tuple_element<index,Tuple> type; };

	template<size_t index,typename Tuple> inline auto& get(Tuple& input) { return std::get<index>(input); }
	template<size_t index,typename Tuple> inline auto& get(const Tuple& input) { return std::get<index>(input); }
}

#endif //USE_FASTL


#ifdef FASTL_EXPOSE_PLAIN_ALIAS

template< typename ... Ts > using tuple = fastl::tuple<Ts...>;

template<size_t index, typename Tuple> struct tuple_element { typedef typename fastl::tuple_element<index, Tuple> type; };

template<size_t index, typename Tuple> inline auto& get(Tuple& input) { return fastl::get<index>(input); }
template<size_t index, typename Tuple> inline auto& get(const Tuple& input) { return fastl::get<index>(input); }

#endif //FASTL_EXPOSE_PLAIN_ALIAS
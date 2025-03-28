#pragma once

#include <common/defines.h>
#include <common/assert.h> // for _com_assert
#include <common/BaseDefines.hpp>
#include <common/Concepts.hpp>
#include <common/Bool.hpp> // for com::Bool

#include <type_traits> // for std::is_reference and std::is_pointer
#include <utility>
#include <iterator> /* for std::advance */
#include <unordered_map> // for std::unordered_map
#include <algorithm> // for std::find and std::transform
#include <filesystem> // for std::filesystem
#include <iterator> // for std::iterator_traits<>
#include <iostream>
#include <span> // for std::span<>
#include <string_view> // for std::string_view

#define _DBG_LINE_ std::cout << __FILE__ << ":" << __LINE__ << std::endl

namespace com
{
	using size_t = com_size_t;

	// Only used in Event.hpp and OrderedEvent.hpp
	struct no_publish_ptr_t { };

	template<typename T> T& null_reference() { return *reinterpret_cast<T*>(NULL); }

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wreturn-local-addr"
		template<typename T>
		T& garbage_reference()
		{
			char bytes[sizeof(T)];
			return *reinterpret_cast<T*>(bytes);
		}
	#pragma GCC diagnostic pop

	template<typename EnumClassType>
	constexpr typename std::underlying_type<EnumClassType>::type EnumClassToInt(EnumClassType value)
	{
		return static_cast<typename std::underlying_type<EnumClassType>::type>(value);
	}

	template<typename EnumClassType, typename IntegerType>
	constexpr EnumClassType IntToEnumClass(IntegerType intValue)
	{
		static_assert(std::is_same<typename std::underlying_type<EnumClassType>::type,IntegerType>::value);
		return static_cast<EnumClassType>(intValue);
	}

	template<typename EnumClassType, typename IntegerType>
	constexpr typename std::underlying_type<EnumClassType>::type CastIntToUnderlyingType(IntegerType intValue)
	{
		return static_cast<typename std::underlying_type<EnumClassType>::type>(intValue);
	}

	template<typename EnumClassType, typename IntegerType>
	constexpr EnumClassType ForceIntToEnumClass(IntegerType intValue)
	{
		return IntToEnumClass<EnumClassType>(CastIntToUnderlyingType<EnumClassType, IntegerType>(intValue));
	}

	template<typename EnumClassType1, typename EnumClassType2>
	constexpr bool EnumClassHasFlag(const EnumClassType1 bits, const EnumClassType2 flag) noexcept
	{
		static_assert(sizeof(EnumClassType1) >= sizeof(EnumClassType2));
		return HAS_FLAG(EnumClassToInt<EnumClassType1>(bits), EnumClassToInt<EnumClassType2>(flag));
	}

	template<typename T>
	class OptionalReference
	{
	private:
		T& m_value;
		bool m_hasValue;
	
	public:
		OptionalReference() : m_value(garbage_reference<T>()), m_hasValue(false) { }
		OptionalReference(T& value) : m_value(value), m_hasValue(true) { }
		
		OptionalReference(OptionalReference& ref) : m_value(ref.m_value), m_hasValue(ref.m_hasValue) { }
		OptionalReference& operator=(T& value) { m_value = value; m_hasValue = true; return *this; }
	
		T& value() noexcept { return m_value; }
		operator bool() const noexcept { return m_hasValue; }
		bool hasValue() const noexcept { return m_hasValue; }
		bool has_value() const noexcept { return hasValue(); }
		T& operator*() noexcept { return m_value; }
		T* operator->() noexcept { return &m_value; }
	};

	template<typename T>
	class Optional
	{
	private:
		T m_value;
		bool m_hasValue;
	
	public:
		Optional() noexcept : m_hasValue(false) { }
		template<typename... Args>
		Optional(Args... args) : m_value(std::forward(args)...), m_hasValue(true) { }
		Optional(T&& value) : m_value(std::move(value)), m_hasValue(true) { }
		Optional(T& value) noexcept : m_value(value), m_hasValue(true) { }
	
		Optional(Optional&& optional) : m_value(std::move(optional.m_value)), m_hasValue(optional.m_hasValue) { }
		Optional(Optional& optional) noexcept : m_value(optional.m_value), m_hasValue(optional.m_hasValue) { }
		Optional& operator=(Optional& optional)
		{
			m_hasValue = optional.m_hasValue;
			if(m_hasValue)
				m_value = optional.m_value;
			return *this; 
		}
		Optional& operator=(Optional&& optional)
		{
			m_hasValue = optional.m_hasValue;
			if(m_hasValue)
				m_value = std::move(optional.m_value);
			return *this;
		}
		Optional& operator=(T& value)
		{
			m_value = value;
			m_hasValue = true;
		}
		Optional& operator=(T&& value)
		{
			m_value = std::move(value);
			m_hasValue = true;
		}
	
		T& value() noexcept { return m_value; }
		operator bool() const noexcept { return m_hasValue; }
		bool has_value() const noexcept { return m_hasValue; }
		T& operator*() noexcept { return m_value; }
		T* operator->() noexcept { return &m_value; }
	};

	template<typename T1, typename T2>
	class Pair
	{
	public:
		T1 first;
		T2 second;
	
		Pair() = default;
		Pair(T1& _first, T2& _second) : first(_first), second(_second) { }
		Pair(T1&& _first, T2&& _second) : first(std::move(_first)), second(std::move(_second)) { }
	
		Pair(Pair& pair) : first(pair.first), second(pair.second) { }
		Pair(Pair&& pair) : first(std::move(pair.first)), second(std::move(pair.second)) { }
	
		Pair& operator=(Pair& pair)
		{
			first = pair.first;
			second = pair.second;
		}
	
		Pair& operator=(Pair&& pair)
		{
			first = std::move(pair.first);
			second = std::move(pair.second);
		}
	};

	template<typename TKey, typename TValue>
	typename std::unordered_map<TKey, TValue>::iterator unordered_map_find(std::unordered_map<TKey, TValue>& map, const TKey& key) noexcept
	{
		auto it = map.find(key);
		_com_assert(it != map.end());
		return it;
	}

	template<typename ContainerType>
	concept FindErasable = requires(ContainerType& container)
	{
		typename ContainerType::value_type;
		{ container.begin() } -> std::same_as<typename ContainerType::iterator>;
		{ container.end() } -> std::same_as<typename ContainerType::iterator>;
	};

	template<FindErasable ContainerType>
	bool find_erase(ContainerType& container, typename ContainerType::value_type element) noexcept
	{
		auto it = std::find(container.begin(), container.end(), element);
		if(it != container.end())
		{
			container.erase(it);
			return true;
		}
		return false;
	}

	template<typename T>
	concept HasKeyType = requires { typename T::key_type; };

	template<typename ContainerType, typename KeyType>
	concept FindEraseContainer = HasKeyType<ContainerType> && requires(ContainerType& container, typename ContainerType::iterator it)
	{
		container.find(std::declval<KeyType>());
		container.erase(it);
	};

	template<HasKeyType ContainerType, typename KeyType = typename ContainerType::key_type>
	requires FindEraseContainer<ContainerType, KeyType>
	bool find_erase(ContainerType& container, const KeyType& key) noexcept
	{
		auto it = container.find(key);
		if(it != container.end())
		{
			container.erase(it);
			return true;
		}
		return false;
	}

	template<typename T>
	concept HasValueType = requires { typename T::value_type; };

	template<typename ContainerType, typename KeyType>
	concept FindContainer = HasKeyType<ContainerType> && HasValueType<ContainerType> && requires(ContainerType& container)
	{ 
		typename ContainerType::mapped_type; 
		container.find(std::declval<KeyType>());
		container.end();
		{ container.find(std::declval<KeyType>()) != container.end() } -> std::same_as<bool>;
	};

	template<HasKeyType ContainerType, typename KeyType = typename ContainerType::key_type>
	requires FindContainer<ContainerType, KeyType>
	typename std::conditional<
				std::is_const<typename std::remove_reference<ContainerType>::type>::value,
				const typename ContainerType::mapped_type&,
				typename ContainerType::mapped_type&>::type
	find_value(ContainerType& container, const KeyType& key) noexcept
	{
		auto it = container.find(key);
		_com_assert(it != container.end());
		return it->second;
	}

	// Finds a value inside a container or returns a default (or) value passed as the third argument to this function
	// Returns: Reference to either the found value or the 'orValue'
	template<HasKeyType ContainerType, typename KeyType = typename ContainerType::key_type, typename ReturnValueType = 
		typename std::conditional<
				std::is_const<typename std::remove_reference<ContainerType>::type>::value,
				const typename ContainerType::mapped_type,
				typename ContainerType::mapped_type>::type>
	requires FindContainer<ContainerType, KeyType>
	ReturnValueType& find_value_or(ContainerType& container, const KeyType& key, ReturnValueType& orValue) noexcept
	{
		auto it = container.find(key);
		if(it != container.end())
			return it->second;
		return orValue;
	}

	template<HasKeyType ContainerType, typename KeyType = typename ContainerType::key_type>
	requires FindContainer<ContainerType, KeyType>
	OptionalReference<typename std::conditional<
				std::is_const<typename std::remove_reference<ContainerType>::type>::value,
				const typename ContainerType::mapped_type,
				typename ContainerType::mapped_type>::type>
	try_find_value(ContainerType& container, const KeyType& key) noexcept
	{
		auto it = container.find(key);
		if(it != container.end())
			return { it->second };
		else
			return { };
	}

	template<typename T>
	concept HasMappedType = requires { typename T::mapped_type; };

	template<typename T>
	concept InsertFindMapContainer = HasKeyType<T> && HasValueType<T> && HasMappedType<T> &&
	requires(T& cont)
	{
		{ cont.insert(typename T::value_type { com::decllval<typename T::key_type>(), std::declval<typename T::mapped_type>() }) };
		{ try_find_value(cont, com::decllval<typename T::key_type>()) };
	};

	template<typename T>
	struct GetViewType
	{
		typedef T type;
	};

	template<>
	struct GetViewType<std::string> { typedef std::string_view type; };

	// Works on std::unordered_map<> type containers which has insert() and find()
	// If key value pair already exists then it only overwrites value of the already existing pair with the value passed as the third argument to this function
	// Otherwise, it inserts a new key value pair
	// Returns: com::True if new key value pair has been inserted, otherwise com::False
	template<InsertFindMapContainer ContainerType, typename KeyType, typename MappedType = typename ContainerType::mapped_type>
	requires(com::SameAsAny<KeyType, typename ContainerType::key_type, typename GetViewType<typename ContainerType::key_type>::type>)
	com::Bool insert_or_set(ContainerType& container, const KeyType& key, MappedType value) noexcept
	{
		auto foundValue = try_find_value(container, key);
		if(foundValue)
		{
			*foundValue = value;
			return com::False;
		}
		container.insert({ key, std::move(value) });
		return com::True;
	}

	template<template<typename> typename STLContainerType, typename T, typename IndexType>
	typename STLContainerType<T>::iterator GetIteratorFromIndex(STLContainerType<T>& stlContainer, IndexType index)
	{
		typename STLContainerType<T>::iterator it = stlContainer.begin();
		std::advance(it, index);
		return it;
	}

	template<template<typename> typename STLContainerType, typename T, typename IndexType>
	typename STLContainerType<T>::const_iterator GetIteratorFromIndex(const STLContainerType<T>& stlContainer, IndexType index)
	{
		typename STLContainerType<T>::const_iterator it = stlContainer.begin();
		std::advance(it, index);
		return it;
	}

	template<typename T, typename FactorType = f32>
	constexpr T Lerp(T a, T b, FactorType t) noexcept
	{
		return a * (static_cast<FactorType>(1) - t) + b * t;
	}

	template<typename FactorType, typename T>
	constexpr FactorType InverseLerp(T value, T a, T b) noexcept
	{
		return (value - a) / static_cast<FactorType>(b - a);
	}

	// NOTE: this checks only for exclusive range
	template<typename T>
	constexpr bool LiesBetween(T value, T min, T max) noexcept
	{
		return (value > min) && (value < max);
	}
	template<typename T>
	constexpr bool LiesBetweenInc(T value, T min, T max) noexcept
	{
		return (value >= min) && (value <= max);
	}

	template<typename T1, typename T2>
	constexpr typename std::common_type<T1, T2>::type min(T1 v1, T2 v2) noexcept
	{
		return (v1 < v2) ? v1 : v2;
	}

	template<typename T1, typename T2>
	constexpr typename std::common_type<T1, T2>::type max(T1 v1, T2 v2) noexcept
	{
		return (v1 > v2) ? v1 : v2;
	}

	template<typename T, std::size_t N>
	constexpr std::size_t array_size(const T (&arr)[N]) noexcept { return N; }

	// Example:
    // std::filesystem::path path = "myFolder/myAnotherFolder/value/";
    // std::cout << path.relative_path() << std::endl;
    // std::cout << path.relative_path().relative_path() << std::endl;
    // std::cout << trim_front(path) << std::endl;
    // std::cout << trim_front(trim_front(path)) << std::endl;
    // std::cout << trim_front(trim_front(trim_front(path))) << std::endl;
	//
	//	Output:
	// 	"myFolder/myAnotherFolder/value/"
	// 	"myFolder/myAnotherFolder/value/"
	// 	"myAnotherFolder/value/"
	// 	"value/"
	// 	""
	std::filesystem::path trim_front(const std::filesystem::path& path) noexcept;
	// Example:
	// std::filesystem::path path { "C:/4-Projects/value/myFolder" };
    // std::cout << trim_back(path) << std::endl;
    // std::filesystem::path path2 { "C:/5-Projects/myemptyfolder///" };
    // std::cout << trim_back(path2) << std::endl;
    // 
    // Output:
    //	"C:/4-Projects/value"
	// 	"C:/5-Projects/myemptyfolder"
	std::filesystem::path trim_back(const std::filesystem::path& path) noexcept;

	template<typename T>
	concept RefOrPtrType = std::is_pointer_v<T> || std::is_reference_v<T>;


	// <begin> Copied from https://stackoverflow.com/questions/45948835/stdis-base-of-and-virtual-base-class (second answer)
	// First, a type trait to check whether a type can be static_casted to another    
	template <typename From, typename To, typename = void>
	struct can_static_cast: std::false_type{};
	
	template <typename From, typename To>
	struct can_static_cast<From, To, std::void_t<decltype(static_cast<To>(std::declval<From>()))>>: std::true_type{};
	
	// Then, we apply the fact that a virtual base is first and foremost a base,
	// that, however, cannot be static_casted to its derived class.
	template <typename Base, typename Derived>
	struct is_virtual_base_of: std::conjunction<
	    std::is_base_of<Base, Derived>, 
	    std::negation<can_static_cast<Base*, Derived*>>
	>{};
	// <end> TODO: switch to C++26, it has out of the box type trait : std::is_virtual_base_of

	template<RefOrPtrType T, RefOrPtrType U>
	static INLINE_IF_RELEASE_MODE T iknow_down_cast(U value) noexcept
	{
		#ifdef COMMON_DEBUG
		T _value = dynamic_cast<T>(value);
		if constexpr (std::is_pointer_v<T>)
		{
			_com_assert(_value != NULL);
		}
		return _value;
		#else
		if constexpr (!is_virtual_base_of<U, T>::value)
			return static_cast<T>(value);
		else // Fallback to dynamic_cast<>
			return dynamic_cast<T>(value);
		#endif
	}

	template<typename T>
	concept ValueType = !std::is_reference_v<T> && !std::is_pointer_v<T>;

	template<ValueType T>
	typename std::remove_const<T>::type cast_away_const(T&& value) noexcept
	{
		return const_cast<typename std::remove_const<T>::type>(std::forward<T&&>(value));
	}

	template<typename T>
	concept PtrType = std::is_pointer_v<T>;

	template<PtrType T>
	struct ptr_remove_const
	{
		typedef 
		typename std::add_pointer<
			typename std::remove_const<
				typename std::remove_pointer<T>::type
				>::type
		>::type no_const_type;
		typedef 
		typename std::conditional<
			std::is_const<T>::value,
			typename std::add_const<no_const_type>::type,
			no_const_type
		>::type type;
	};

	template<PtrType T>
	typename ptr_remove_const<T>::type cast_away_const(T&& value) noexcept
	{
		return const_cast<typename ptr_remove_const<T>::type>(std::forward<T&&>(value));
	}

	template<typename T>
	concept RefType = std::is_reference_v<T>;

	template<RefType T>
	struct ref_remove_const
	{
		typedef 
		typename std::conditional<
		std::is_lvalue_reference<T>::value, 
			typename std::add_lvalue_reference<
				typename std::remove_const<
					typename std::remove_reference<T>::type
				>::type
			>::type,
			typename std::add_rvalue_reference<
				typename std::remove_const<
					typename std::remove_reference<T>::type
				>::type
			>::type
		>::type type;
	};

	template<RefType T>
	typename ref_remove_const<T>::type cast_away_const(T&& value) noexcept
	{
		return const_cast<typename ref_remove_const<T>::type>(std::forward<T&&>(value));
	}

	template<typename T>
	struct TransparentHash
	{
		using is_transparent = void;

		constexpr std::size_t operator()(const T& value) const noexcept
		{
			return std::hash<T> { } (value);
		}
	};

	template<typename T>
	struct TransparentEqualTo
	{
		using is_transparent = void;
		constexpr bool operator()(const T& value1, const T& value2) const noexcept
		{
			return std::equal_to<T> { } (value1, value2);
		}
	};

	template<typename KeyType, typename ValueType, ViewType<KeyType> KeyViewType = typename GetViewType<KeyType>::type>
	struct _unordered_map
	{
		typedef std::unordered_map<KeyType, ValueType, com::TransparentHash<KeyViewType>, com::TransparentEqualTo<KeyViewType>> type;
	};

	template<typename KeyType, typename ValueType, ViewType<KeyType> KeyViewType = typename GetViewType<KeyType>::type>
	using unordered_map = typename _unordered_map<KeyType, ValueType, KeyViewType>::type;

	template<typename T>
	struct is_std_pair : std::false_type { };

	template<typename T1, typename T2>
	struct is_std_pair<std::pair<T1, T2>> : std::true_type { };

	template<typename T>
	concept IsStdPair = is_std_pair<T>::value;

	template<typename T>
	concept KeyValueIterator = HasValueType<T> && IsStdPair<typename T::value_type> && std::indirectly_readable<T>;

	template<KeyValueIterator KVIterator>
	class KeyIterator
	{
	public:
		typedef typename std::iterator_traits<KVIterator>::value_type::first_type value_type;
	private:
		KVIterator m_it;
	public:
		KeyIterator(KVIterator it) noexcept : m_it(it) { }

		KeyIterator& operator++() noexcept
		{
			++m_it;
			return *this;
		}
		KeyIterator& operator--() noexcept
		{
			++m_it;
			return *this;
		}
		KeyIterator& operator++(int) noexcept
		{
			++m_it;
			return *this;
		}
		KeyIterator& operator--(int) noexcept
		{
			--m_it;
			return *this;
		}

		value_type& operator*() noexcept
		{
			return m_it->first;
		}

		value_type& operator->() noexcept
		{
			return m_it->first;
		}

		bool operator==(const KeyIterator<KVIterator>& it) noexcept
		{
			return m_it == it.m_it;
		}
		bool operator!=(const KeyIterator<KVIterator>& it) noexcept
		{
			return m_it != it.m_it;
		}
	};

	template<typename iterator>
	class Iteratable
	{
	private:
		iterator m_begin;
		iterator m_end;
	public:
		Iteratable(iterator begin, iterator end) noexcept : m_begin(begin), m_end(end) { }
		iterator begin() noexcept { return m_begin; }
		iterator end() noexcept { return m_end; }
	};

	template<typename T>
	concept HasBeginEndKeyValueIt = requires(T& t)
	{
		typename T::iterator;
		{ t.begin() } -> KeyValueIterator;
		{ t.end() } -> KeyValueIterator;
	} && KeyValueIterator<typename T::iterator>;

	template<HasBeginEndKeyValueIt T>
	class KeyIteratable : public Iteratable<KeyIterator<typename T::iterator>>
	{
	public:
		KeyIteratable(T& container) : Iteratable<KeyIterator<typename T::iterator>>(container.begin(), container.end()) { }
		KeyIteratable(typename T::iterator begin, typename T::iterator end) : Iteratable<KeyIterator<typename T::iterator>>(KeyIterator { begin }, KeyIterator { end }) { }
	};

	template<typename T>
	class LinkedListNodeBase
	{
	protected:
		T* m_next;
		T* m_prev;
	public:
		LinkedListNodeBase() noexcept : m_next(com::null_pointer<T>()), m_prev(com::null_pointer<T>()) { }
		T* getNext() noexcept { return m_next; }
		const T* getNext() const noexcept { return m_next; }
		void setNext(T* node) noexcept { m_next = node; }
		T* getPrev() noexcept { return m_prev; }
		const T* getPrev() const noexcept { return m_prev; }
		void setPrev(T* node) noexcept { m_prev = node; }
	};

	template<typename T>
	static constexpr std::pair<T, T> GetMinMax(T a, T b) noexcept
	{
		if(a > b)
			return { b, a };
		else
			return { a, b };
	}

	template<std::integral IntegerType>
	static constexpr com::Bool IsOdd(IntegerType value) noexcept
	{
		return com::Bool { static_cast<bool>(value & 0x1) };
	}

	template<typename T>
	std::string_view span_to_string_view(std::span<T>& span) noexcept
	{
		static_assert(sizeof(T) == sizeof(char));
		if constexpr(std::is_const_v<T>)
			return std::string_view { reinterpret_cast<const char*>(span.data()), span.size() };
		else return std::string_view { reinterpret_cast<char*>(span.data()), span.size() };
	}

	template<typename T>
	const std::string_view span_to_string_view(const std::span<T>& span) noexcept
	{
		static_assert(sizeof(T) == sizeof(char));
		return std::string_view { reinterpret_cast<const char*>(span.data()), span.size() };
	}

	// Fully Traverses a doubly-linked list starting from anywhere in the linked list
	// node: pointer to a doubly LinkedListNode, it may be in the middle; can be null, in that case this function does nothing
	// visitor: called for each node being visited
	template<com::concepts::LinkedListNode T, com::concepts::UnaryVisitor<T*> VisitorType>
	void TraverseLinkedListBiDirect(T* node, VisitorType visitor) noexcept
	{
		if(!node) return;
		auto* left = node->getPrev();
		while(left)
		{
			// NOTE: we need to pointer to the previous node before even calling 'visitor', that's because the visitor might delete the node object
			// Therefore, dereferencing the deleted node won't be possible later
			auto* nextLeft = left->getPrev();
			visitor(left);
			left = nextLeft;
		}
		while(node)
		{
			auto* nextRight = node->getNext();
			visitor(node);
			node = nextRight;
		}
	}
	
	// Fully traverses a forward-linked list starting from the head (root node)
	// node: pointer to the head node; call be null, in that case this function does nothing
	// visitor: called for each node being visisted
	template<com::concepts::ForwardLinkedListNode T, com::concepts::UnaryVisitor<T*> VisitorType>
	void TraverseLinkedList(T* node, VisitorType visitor) noexcept
	{
		while(node)
		{
			auto* nextNode = node->getNext();
			visitor(node);
			node = nextNode;
		}
	}

	template<com::concepts::LinkedListNodeSetters T>
	void RemoveLinkedListNode(T* node, com::Bool isSetNull = com::True) noexcept
	{
		if(node->getPrev())
			node->getPrev()->setNext(node->getNext());
		if(node->getNext())
			node->getNext()->setPrev(node->getPrev());
		if(isSetNull)
		{
			node->setNext(com::null_pointer<T>());
			node->setPrev(com::null_pointer<T>());
		}
	}

	template<com::concepts::LinkedListNodeSetters T>
	void InsertLinkedListNodeAfter(T* node, T* after) noexcept
	{
		if(auto afterNext = after->getNext())
		{
			afterNext->setPrev(node);
			node->setNext(afterNext);
		}
		node->setPrev(after);	
		after->setNext(node);
	}

	template<com::concepts::LinkedListNodeSetters T>
	void InsertLinkedListNodeBefore(T* node, T* before) noexcept
	{
		if(auto beforePrev = before->getPrev())
		{
			beforePrev->setNext(node);
			node->setPrev(beforePrev);
		}
		node->setNext(before);
		before->setPrev(node);
	}

	template<com::concepts::BackwardLinkedListNode T>
	u32 GetLinkedListNodeIndex(const T* node) noexcept
	{
		u32 index = 0;
		node = node->getPrev();
		while(node)
		{
			++index;
			node = node->getPrev();
		}
		return index;
	}

	template<com::concepts::BackwardLinkedListNode T>
	T* GetLinkedListRoot(T* node) noexcept
	{
		while(node && node->getPrev())
			node = node->getPrev();
		return node;
	}

	template<typename T>
	constexpr com::Bool is_nullptr(T* ptr) noexcept
	{
		return com::Bool { ptr == com::null_pointer<T>() };
	}

	// Removes pointer and reference traits of a type
	template<typename T>
	struct remove_pointer_and_reference
	{
		using type = typename std::remove_reference<typename std::remove_pointer<T>::type>::type;
	};

	// Tells if a type refers to a const "value"
	// NOTE: It returns false for reference to non-const types, unlike std::is_const<>, which returns true
	template<typename T>
	struct is_refers_to_const_value
	{
		static constexpr bool value = std::is_const<typename remove_pointer_and_reference<T>::type>::value;
	};

	// Checks if two types are identical if cv-qualification is ignored
	template<typename T1, typename T2>
	struct is_same_nocv
	{
		static constexpr bool value = std::is_same<typename std::remove_cv<T1>::type, typename std::remove_cv<T2>::type>::value;
	};

	// ------------------------- to_upper() ------------------------------------------------------
	// Converts each character in a string (std::string or std::string_view) to upper case equivalent
	// WARN: It does modifies the original data if passed as std::string.
	// NOTE:
	//	1. If passed std::string_view or const std::string then it returns std::string (a copy and transformed object)
	// 	2. If passed (non-const) std::string then it returns reference to it, that means it doesn't creates any new std::string object.
	template<typename T, 
			bool isConstData = 
			is_same_nocv<typename std::remove_reference<T>::type, 
							std::string_view>::value
			|| is_refers_to_const_value<T>::value>
	static typename std::conditional<isConstData, std::string, T&>::type to_upper(T& str)
	{
		auto toupper = [](T::value_type c){ return std::toupper(static_cast<char>(c)); };
		if constexpr(isConstData)
		{
			std::string strCopy { str };
			std::transform(str.begin(), str.end(), strCopy.begin(), toupper);
			return strCopy;
		}
		else
		{
			std::transform(str.begin(), str.end(), str.begin(), toupper);
			return str;
		}
	}
	template<typename T>
	static decltype(auto) to_upper(T&& str)
	{
		return to_upper<const T>(str);
	}
	static decltype(auto) to_upper(const char* str)
	{
		return to_upper(std::string_view { str });
	}
	// ---------------------------------------------------------------------------------------
}


template<typename EnumClassType>
constexpr EnumClassType operator&(const EnumClassType a, const EnumClassType b) noexcept
{
	return com::IntToEnumClass<EnumClassType, typename std::underlying_type<EnumClassType>::type>(com::EnumClassToInt<EnumClassType>(a) & com::EnumClassToInt<EnumClassType>(b));
}

template<typename EnumClassType>
constexpr EnumClassType operator|(const EnumClassType a, const EnumClassType b) noexcept
{
	return com::IntToEnumClass<EnumClassType, typename std::underlying_type<EnumClassType>::type>(com::EnumClassToInt<EnumClassType>(a) | com::EnumClassToInt<EnumClassType>(b));
}

template<typename EnumClassType>
constexpr EnumClassType operator|=(EnumClassType& a, const EnumClassType b) noexcept
{
	a = com::IntToEnumClass<EnumClassType, typename std::underlying_type<EnumClassType>::type>(com::EnumClassToInt<EnumClassType>(a) | com::EnumClassToInt<EnumClassType>(b));
	return a;
}

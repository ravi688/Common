#pragma once

#include <common/defines.h>
#include <common/assert.h> // for _com_assert

#include <type_traits> // for std::is_reference and std::is_pointer
#include <utility>
#include <iterator> /* for std::advance */
#include <unordered_map> // for std::unordered_map
#include <algorithm> // for std::find
#include <filesystem> // for std::filesystem
#include <iostream>

#define _DBG_LINE_ std::cout << __FILE__ << ":" << __LINE__ << std::endl

namespace com
{
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

	template<typename EnumClassType>
	constexpr bool EnumClassHasFlag(const EnumClassType bits, const EnumClassType flag) noexcept
	{
		return HAS_FLAG(EnumClassToInt<EnumClassType>(bits), EnumClassToInt<EnumClassType>(flag));
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
		container.begin();
		container.end();
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

	template<typename ContainerType>
	concept AssociativeContainer = requires(ContainerType& container, 
				typename ContainerType::key_type key,
				typename ContainerType::iterator it)
	{
		typename ContainerType::key_type;
		container.find(key);
		container.erase(it);
	};

	template<AssociativeContainer ContainerType>
	bool find_erase(ContainerType& container, typename ContainerType::key_type key) noexcept
	{
		auto it = container.find(key);
		if(it != container.end())
		{
			container.erase(it);
			return true;
		}
		return false;
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

	template<RefOrPtrType T, RefOrPtrType U>
	static INLINE_IF_RELEASE_MODE T iknow_down_cast(U value) noexcept
	{
		#ifdef GLOBAL_DEBUG
		T _value = dynamic_cast<T>(value);
		if (std::is_pointer_v<T>)
		{
			_com_assert(_value != NULL);
		}
		return _value;
		#else
		return static_cast<T>(value);
		#endif
	}

	template<typename T>
	concept ValueType = !std::is_reference_v<T>;

	template<ValueType T>
	typename std::remove_const<T>::type cast_away_const(T&& value) noexcept
	{
		return const_cast<typename std::remove_const<T>::type>(std::forward<T&&>(value));
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

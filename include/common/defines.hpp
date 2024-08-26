#pragma once

#include <common/defines.h>

#include <type_traits>
#include <utility>
#include <iterator> /* for std::advance */

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

	template<typename Iterator, class UnaryPredicate>
	bool erase_first_if(Iterator begin, Iterator end, UnaryPredicate predicate)
	{
		for(Iterator& it = begin; it != end; it++)
			if(predicate(*it))
			{
				for(Iterator& _it = it; ++_it != end;)
					*it++ = std::move(*_it);
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

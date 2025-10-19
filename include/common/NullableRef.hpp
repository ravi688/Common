#pragma once

#include <type_traits>

namespace com
{
	template<typename T>
	T& garbage_reference()
	{
		static typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
		return *reinterpret_cast<T*>(&storage);
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
		
		OptionalReference& operator=(const T& value) { m_value = value; m_hasValue = true; return *this; }

		T& value() noexcept { return m_value; }
		operator bool() const noexcept { return m_hasValue; }
		bool hasValue() const noexcept { return m_hasValue; }
		bool has_value() const noexcept { return hasValue(); }
		T& operator*() noexcept { return m_value; }
		T* operator->() noexcept { return &m_value; }
	};

	template<typename T>
	using NullableRef = OptionalReference<T>;
}

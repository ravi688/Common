#pragma once

#include <cstddef> // for NULL
#include <common/defines.hpp> // for com::null_pointer<>
#include <common/Reference.hpp> // for com::Reference<>

namespace com
{
	#define PTR_REFERENCE_DERIVED_CLASS(DerivedClass) \
	public: \
		DerivedClass(Reference::HandleType handle) noexcept : com::PtrReference<typename std::remove_pointer<Reference::HandleType>::type>(handle) { } \
		DerivedClass() noexcept { } \
		DerivedClass(const DerivedClass& reference) : com::PtrReference<typename std::remove_pointer<Reference::HandleType>::type>(static_cast<const com::PtrReference<typename std::remove_pointer<Reference::HandleType>::type>&>(reference)) { } \
		DerivedClass& operator =(const DerivedClass reference) noexcept \
		{ \
			m_handle = reference.m_handle; \
			return *this; \
		} \
		bool operator ==(const DerivedClass reference) const noexcept \
		{ \
			return m_handle == reference.m_handle; \
		} \
		bool operator !=(const DerivedClass reference) const noexcept \
		{ \
			return !operator==(reference); \
		} \
		explicit operator Reference::HandleType&() noexcept { return m_handle; } \
		explicit operator const Reference::HandleType&() const noexcept { return m_handle; } \
		explicit operator Reference::HandleType() const noexcept { return m_handle; } \
		operator bool() const noexcept { return m_handle == NULL; }

	template<typename T>
	using PtrReference = Reference<T*, com::null_pointer<T>()>;
}

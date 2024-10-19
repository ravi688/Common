#pragma once

#include <cstddef> // for NULL
#include <common/defines.hpp> // for com::null_pointer<>
#include <common/Reference.hpp> // for com::Reference<>

namespace com
{
	#define PTR_REFERENCE_DERIVED_CLASS(DerivedClass) \
	public: \
		DerivedClass() noexcept { } \
		DerivedClass(Reference::HandleType handle) noexcept : com::PtrReference<typename std::remove_pointer<Reference::HandleType>::type>(handle) { } \
		DerivedClass(const DerivedClass& reference) : com::PtrReference<typename std::remove_pointer<Reference::HandleType>::type>(static_cast<const com::PtrReference<typename std::remove_pointer<Reference::HandleType>::type>&>(reference)) { } \
		DerivedClass& operator =(const Reference::HandleType& handle) noexcept \
		{ \
			Reference::operator=(handle); \
			return *this; \
		} \
		DerivedClass& operator =(const DerivedClass reference) noexcept \
		{ \
			Reference::operator=(reference); \
			return *this; \
		} \
		bool operator ==(const DerivedClass reference) const noexcept \
		{ \
			return Reference::operator==(reference); \
		} \
		bool operator !=(const DerivedClass reference) const noexcept \
		{ \
			return Reference::operator!=(reference); \
		} \
		explicit operator Reference::HandleType&() noexcept \
		{\
		 	return Reference::operator Reference::HandleType&(); \
		} \
		explicit operator const Reference::HandleType&() const noexcept \
		{ \
			return Reference::operator const Reference::HandleType&(); \
		} \
		explicit operator Reference::HandleType() const noexcept \
		{ \
			return Reference::operator Reference::HandleType(); \
		} \
		explicit operator bool() const noexcept \
		{ \
			return Reference::operator bool(); \
		}

	template<typename T>
	using PtrReference = Reference<T*, com::null_pointer<T>()>;
}

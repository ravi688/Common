#pragma once

#include <type_traits> // for std::add_lvalue_reference<>

namespace com
{
	template<typename Type>
	constexpr Type* null_pointer() noexcept
	{
		// NOTE: reinterpert_cast<Type*>(0) leads to non-constant expression!
		return static_cast<Type*>(0);
	}

	// This creates an lvalue reference in unevaluated contexts, similar to std::declval which does that for rvalue references.
	// NOTE: This won't work in evaluated contexts.
	template<typename T>
	typename std::add_lvalue_reference<T>::type decllval() noexcept;
}

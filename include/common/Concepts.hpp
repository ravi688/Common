#pragma once

#include <common/BaseDefines.hpp> // for com::decllval<>
#include <concepts> // for std::is_convertible and std::is_constructible

namespace com
{
	template<typename _ViewType, typename Type>
	concept ViewType = std::is_convertible<Type, _ViewType>::value && std::is_constructible<Type, const _ViewType&>::value;

	template<typename T, typename... U>
	concept SameAsAny = (std::same_as<T, U> || ...);

	template<typename T>
	concept Number = std::floating_point<T> || std::integral<T>;

	template<typename T, typename ValueType>
	concept CompareFunction = requires ()
	{
		{ T { } (std::declval<ValueType>(), std::declval<ValueType>()) } -> std::same_as<bool>;
	};

	namespace concepts
	{
		template<typename T, typename ValueType>
		concept UnaryVisitor = requires(T& instance)
		{
			{ instance (std::declval<ValueType>()) } -> std::same_as<void>;
		} || requires(T& instance)
		{
			{ instance (com::decllval<ValueType>()) } -> std::same_as<void>;
		};
	}
}

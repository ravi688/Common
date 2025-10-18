
#include <common/Concepts.hpp> // for com::concepts::BufferLike<>
#include <optional> // for std::optional<>
#include <cstring> // for std::memcpy()
#include <type_traits> // for std::is_enum<> and std::underlying_type<>
#include <utility> // for std::tuple<>

namespace com
{
	template<typename T, com::concepts::BufferLike BufferType>
	T __bitcast_buffer_unsafe(const BufferType& buffer, std::unsigned_integral auto targetSize, std::size_t offset = 0)
	{
		T obj;
		std::memcpy(&obj, buffer.data() + offset, targetSize);
		return obj;
	}

	template<typename T, com::concepts::BufferLike BufferType>
	std::optional<T> __bitcast_buffer(const BufferType& buffer, std::unsigned_integral auto targetSize)
	{
		if(buffer.size() != targetSize)
			return { };
		auto obj = __bitcast_buffer_unsafe<T, BufferType>(buffer, targetSize);
		return { std::move(obj) };
	}

	template<typename T, com::concepts::BufferLike BufferType>
	std::optional<T> __bitcast_buffer_relaxed(const BufferType& buffer, std::unsigned_integral auto targetSize)
	{
		if(buffer.size() < targetSize)
			return { };
		auto obj = __bitcast_buffer_unsafe<T, BufferType>(buffer, targetSize);
		return { std::move(obj) };
	}

	template<typename T, com::concepts::BufferLike BufferType>
	std::optional<T> BitCastBufferToEnum(const BufferType& buffer)
	{
		static_assert(std::is_enum<T>::value);
		constexpr auto size = sizeof(typename std::underlying_type<T>::type);
		return __bitcast_buffer<T, BufferType>(buffer, size);	
	}

	template<typename T, com::concepts::BufferLike BufferType>
	std::optional<T> BitCastBufferToEnumRelaxed(const BufferType& buffer)
	{
		static_assert(std::is_enum<T>::value);
		constexpr auto size = sizeof(typename std::underlying_type<T>::type);
		return __bitcast_buffer_relaxed<T, BufferType>(buffer, size);	
	}

	template<typename T>
	std::optional<T> BitCastBuffer(const com::concepts::BufferLike auto& buffer)
	{
		using BufferType = typename std::remove_const<typename std::remove_reference<decltype(buffer)>::type>::type;
		if constexpr (std::is_enum<T>::value)
			return BitCastBufferToEnum<T, BufferType>(buffer);
		else
			return __bitcast_buffer<T, BufferType>(buffer, sizeof(T));
	}

	template<typename T>
	std::optional<T> BitCastBufferRelaxed(const com::concepts::BufferLike auto& buffer)
	{
		using BufferType = typename std::remove_const<typename std::remove_reference<decltype(buffer)>::type>::type;
		if constexpr (std::is_enum<T>::value)
			return BitCastBufferToEnumRelaxed<T, BufferType>(buffer);
		else
			return __bitcast_buffer_relaxed<T, BufferType>(buffer, sizeof(T));
	}

	template<typename T1, typename T2, typename... Args>
	std::optional<std::tuple<T1, T2, Args...>> BitCastBufferPacked(const com::concepts::BufferLike auto& buffer)
	{
		constexpr auto totalSize = sizeof(T1) + sizeof(T2) + (sizeof(Args) + ...);
		if(buffer.size() != totalSize)
			return { };

		using BufferType = typename std::remove_const<typename std::remove_reference<decltype(buffer)>::type>::type;

    	std::size_t offset = 0;
    	auto next_offset = [&](std::size_t size)
    	{
    	    std::size_t old = offset;
    	    offset += size;
    	    return old;
    	};

		std::tuple<T1, T2, Args...> tupleRes
		{
			 __bitcast_buffer_unsafe<T1, BufferType>(buffer, sizeof(T1), next_offset(sizeof(T1))),
			 __bitcast_buffer_unsafe<T2, BufferType>(buffer, sizeof(T2), next_offset(sizeof(T2))),
			 __bitcast_buffer_unsafe<Args, BufferType>(buffer, sizeof(Args), next_offset(sizeof(Args)))...
		};

		return { tupleRes };
	}
}

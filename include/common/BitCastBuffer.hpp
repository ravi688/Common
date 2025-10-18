
#include <common/Concepts.hpp> // for com::concepts::BufferLike<>
#include <optional> // for std::optional<>
#include <cstring> // for std::memcpy()
#include <type_traits> // for std::is_enum<> and std::underlying_type<>

namespace com
{
	template<typename T, com::concepts::BufferLike BufferType>
	std::optional<T> __bitcast_buffer(const BufferType& buffer, std::integral auto targetSize)
	{
		if(buffer.size() != targetSize)
			return { };
		T buf;
		std::memcpy(&buf, buffer.data(), targetSize);
		return { std::move(buf) };
	}

	template<typename T, com::concepts::BufferLike BufferType>
	std::optional<T> BitCastBufferToEnum(const BufferType& buffer)
	{
		static_assert(std::is_enum<T>::value);
		constexpr auto size = sizeof(typename std::underlying_type<T>::type);
		return __bitcast_buffer<T, BufferType>(buffer, size);	
	}

	template<typename T, com::concepts::BufferLike BufferType>
	std::optional<T> BitCastBuffer(const BufferType& buffer)
	{
		if constexpr (std::is_enum<T>::value)
			return BitCastBufferToEnum<T, BufferType>(buffer);
		else
			return __bitcast_buffer<T, BufferType>(buffer, sizeof(T));
	}
}

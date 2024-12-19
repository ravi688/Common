#pragma once

#include <common/utility.h>
#include <common/Bool.hpp> // for com::Bool
#include <bufferlib/buffer.h>

#include <cstddef> // for std::byte
#include <span> // for std::span<>
#include <optional> // for std::optional<>
#include <string_view> // for std::string_view

namespace com
{
	class COMMON_API FileLoadResultBase
	{
	protected:
		void* m_data;
		buf_free_t m_free;
		com::Bool m_isValid;
	public:
		// data : must not be NULL, should point to valid memory blocks and free-able by calling 'm_free'
		// _free: can be NULL, if it is NULL then it is not invoked over m_data to delete that
		FileLoadResultBase(void* data, buf_free_t _free) noexcept : m_data(data), m_free(_free), m_isValid(com::True) { }
		FileLoadResultBase(FileLoadResultBase&& result) noexcept;

		explicit operator bool() const noexcept { return static_cast<bool>(m_isValid); }
		com::Bool isValid() const noexcept { return m_isValid; }
		void destroy() noexcept;
	};

	template<typename T>
	class COMMON_API FileLoadResult : public FileLoadResultBase
	{
	private:
		u32 m_size;
	public:
		// data (std::span<T>) : must hold view to a valid memory block
		// _free: can be NULL, if it is NULL then it is not invoked over m_data to delete that
		FileLoadResult(std::span<T> data, buf_free_t _free) noexcept : FileLoadResultBase(data.data(), _free) { }

		T* data() noexcept { return reinterpret_cast<T*>(m_data); }
		const T* data() const noexcept { return reinterpret_cast<const T*>(m_data); }
		std::span<T> span() noexcept { return { data(), size() }; }
		const std::span<T> span() const noexcept { return { data(), size() }; }
		u32 size() const noexcept { return m_size; }
	};

	typedef FileLoadResult<std::byte> BinaryFileLoadResult;
	typedef FileLoadResult<char> TextFileLoadResult;

	typedef std::function<TextFileLoadResult(const std::string_view)> TextFileLoadCallback;
	typedef std::function<BinaryFileLoadResult(const std::string_view)> BinaryFileLoadCallback;

	COMMON_API BinaryFileLoadResult LoadBinaryFile(const std::string_view filePath) noexcept;
	COMMON_API TextFileLoadResult LoadTextFile(const std::string_view filePath) noexcept;
}
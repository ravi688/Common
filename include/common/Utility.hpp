#pragma once

#include <common/defines.hpp>
#include <common/utility.h>
#include <common/Bool.hpp> // for com::Bool
#include <bufferlib/buffer.h>

#include <cstddef> // for std::byte
#include <span> // for std::span<>
#include <optional> // for std::optional<>
#include <string_view> // for std::string_view
#include <functional> // for std::function<>

namespace com
{
	class COMMON_API FileLoadResultBase
	{
	protected:
		const void* m_data;
		buf_free_t m_free;
		void* m_freeUserData;
		com::Bool m_isValid;
	public:
		FileLoadResultBase() noexcept : m_data(nullptr), m_free(nullptr), m_freeUserData(nullptr), m_isValid(com::False) { }
		// data : must not be NULL, should point to valid memory blocks and free-able by calling 'm_free'
		// _free: can be NULL, if it is NULL then it is not invoked over m_data to delete that
		// freeUserData: can be NULL, but it would passed to 'buf_free_t' as the second argument regardless
		explicit FileLoadResultBase(const void* data, buf_free_t _free, void* freeUserData) noexcept : m_data(data), m_free(_free), m_freeUserData(freeUserData), m_isValid(com::True) { }
		// NOTE: We need to keep it explicit here, otherwise this also resolves with 'const char*' for some reasons
		explicit FileLoadResultBase(const void* data) : FileLoadResultBase(data, nullptr, nullptr) { }
		FileLoadResultBase(FileLoadResultBase&& result) noexcept;

		explicit operator bool() const noexcept { return static_cast<bool>(m_isValid); }
		com::Bool isValid() const noexcept { return m_isValid; }
		void destroy() noexcept;
	};

	template<typename T>
	class COMMON_API FileLoadResult : public FileLoadResultBase
	{
	private:
		const std::span<const T>::size_type m_size;
	public:
		using FileLoadResultBase::FileLoadResultBase;
		FileLoadResult() noexcept : m_size(0) { }
		// data (std::span<T>) : must hold view to a valid memory block
		// _free: can be NULL, if it is NULL then it is not invoked over m_data to delete that
		FileLoadResult(std::span<const T> data, buf_free_t _free, void* freeUserData) noexcept : FileLoadResultBase(data.data(), _free, freeUserData), m_size(data.size()) { }

		const T* data() const noexcept { return reinterpret_cast<const T*>(m_data); }
		std::span<const T> span() const noexcept { return { data(), size() }; }
		std::span<const T>::size_type size() const noexcept { return m_size; }
	};

	typedef FileLoadResult<std::byte> BinaryFileLoadResult;
	typedef FileLoadResult<char> TextFileLoadResult;

	typedef std::function<TextFileLoadResult(const std::string_view)> TextFileLoadCallback;
	typedef std::function<BinaryFileLoadResult(const std::string_view)> BinaryFileLoadCallback;

	COMMON_API BinaryFileLoadResult LoadBinaryFile(const std::string_view filePath) noexcept;
	COMMON_API TextFileLoadResult LoadTextFile(const std::string_view filePath) noexcept;
}
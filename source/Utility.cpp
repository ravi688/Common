#include <common/Utility.hpp>
#include <disk_manager/file_reader.h> // for load_text_from_file() and load_binary_from_file()
#include <bufferlib/buffer.h> // for buf_* functions
#include <cstring>
#include <utility>

namespace com
{
	COMMON_API BinaryFileLoadResult LoadBinaryFile(const std::string_view filePath) noexcept
	{
		char nullTermFilePath[filePath.size() + 1];
		std::memcpy(nullTermFilePath, filePath.data(), filePath.size());
		nullTermFilePath[filePath.size()] = 0;
		buffer_t* fileData = load_binary_from_file(nullTermFilePath);
		if(fileData)
			return { };
		auto freeCallback = buf_get_free_callback(fileData);
		auto freeUserData = buf_get_malloc_callback_user_data(fileData);
		std::span<std::byte> span = com::GetSpanFromBuffer<std::byte>(fileData);
		buf_free_except_data(fileData);
		return { span, freeCallback, freeUserData };
	}

	COMMON_API TextFileLoadResult LoadTextFile(const std::string_view filePath) noexcept
	{
		char nullTermFilePath[filePath.size() + 1];
		std::memcpy(nullTermFilePath, filePath.data(), filePath.size());
		nullTermFilePath[filePath.size()] = 0;
		buffer_t* fileData = load_text_from_file(nullTermFilePath);
		if(fileData)
			return { };
		auto freeCallback = buf_get_free_callback(fileData);
		auto freeUserData = buf_get_malloc_callback_user_data(fileData);
		std::span<char> span = com::GetSpanFromBuffer<char>(fileData);
		buf_free_except_data(fileData);
		return { span, freeCallback, freeUserData };
	}

	FileLoadResultBase::FileLoadResultBase(FileLoadResultBase&& result) noexcept : 
			m_data(std::exchange(result.m_data, nullptr)),
			m_free(std::exchange(result.m_free, nullptr)),
			m_freeUserData(std::exchange(result.m_freeUserData, nullptr)),
			m_isValid(std::exchange(result.m_isValid, com::False))
	{
	}

	void FileLoadResultBase::destroy() noexcept
	{
		if(m_isValid)
		{
			if(m_free)
				m_free(const_cast<void*>(m_data), m_freeUserData);
			m_isValid = com::False;
		}
		else DEBUG_LOG_ERROR("You're trying to destroy an invalid FileLoadResult object");
	}
}
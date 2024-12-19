#include <common/utility.hpp>

#include <disk_manager/file_reader.h> // for load_text_from_file() and load_binary_from_file()

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
		std::span<std::byte> span = GetSpanFromBuffer<std::byte>(fileData);
		buf_free_except_data(fileData);
		return { span, freeCallback };
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
		std::span<char> span = GetSpanFromBuffer<char>(fileData);
		buf_free_except_data(fileData);
		return { span, freeCallback };
	}

	FileLoadResultBase::FileLoadResultBase(FileLoadResultBase&& result) noexcept : 
			m_data(std::exchange(result.m_data, NULL)),
			m_isValid(std::exchange(result.m_isValid, com::False))
	{
	}

	void FileLoadResultBase::destroy() noexcept
	{
		if(m_isValid)
		{
			if(m_free)
				m_free(result->data());
			m_isValid = com::False
		}
		else DEBUG_LOG_ERROR("You're trying to destroy an invalid FileLoadResult object");
	}
}
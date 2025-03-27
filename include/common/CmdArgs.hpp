#pragma once

#include <common/defines.hpp>

namespace com
{
	class CmdArgs
	{
	private:
		inline static u32 m_argc { 0 };
		inline static const char** m_argv { nullptr };
	public:
		template<std::integral ArgCountType>
		static void init(const ArgCountType argc, const char* argv[]) noexcept
		{
			m_argc = static_cast<u32>(argc);
			m_argv = argv;
		}

		static u32 getArgCount() noexcept { return m_argc; }
		// Retuns pointer to const pointer to const char data, so that none of the strings and neither their char data can be modified
		static const char* const* getArgs() noexcept { return m_argv; }
	};
}

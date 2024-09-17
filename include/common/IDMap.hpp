#pragma once

#include <common/id_generator.h> // for id_generator_t
#include <common/defines.hpp> // for com::find_erase
#include <unordered_map> // for std::unordered_map

namespace com
{
	template<typename T>
	class IDMap
	{
	public:
		typedef id_generator_id_type_t IDType;
		static constexpr id_generator_id_type_t InvalidID = ID_GENERATOR_ID_TYPE_MAX;
	private:
		id_generator_t m_id_gen;
		std::unordered_map<id_generator_id_type_t, T> m_map;
	public:
		IDMap() noexcept : m_id_gen(id_generator_create(0, NULL)) { }
		~IDMap() noexcept
		{
			id_generator_destroy(&m_id_gen);
		}
		IDType add(T&& value) noexcept
		{
			auto id = id_generator_get(&m_id_gen);
			m_map.insert({ id, std::forward<T&&>(value) });
			return id;
		}
		T& get(IDType id) noexcept
		{
			auto it = m_map.find(id);
			_com_assert(it != m_map.end());
			return it->second;
		}
		void remove(IDType id) noexcept
		{
			bool result = com::find_erase(m_map, id);
			_com_assert(result);
			id_generator_return(&m_id_gen, id);
		}
	};
}
#pragma once

#include <common/Concepts.hpp> // for com::concepts::UnaryPredicate
#include <common/Bool.hpp> // for com::Bool
#include <vector> // for std::vector

namespace com
{
	template<typename T>
	class UnorderedEraseSafeVectorProxy
	{
	public:
		typedef std::vector<T>::size_type size_type;
		typedef std::vector<T>::value_type value_type;
		typedef std::vector<T>::iterator iterator;
		typedef std::vector<T>::const_iterator const_iterator;
	private:
		std::vector<T> m_data;
		size_type m_curTraverseIndex;
		com::Bool m_isTraversing;
	public:

		UnorderedEraseSafeVectorProxy(const std::initializer_list<T>& initList) noexcept
		: m_data(initList),
			m_curTraverseIndex(0),
			m_isTraversing(com::False) { }
		UnorderedEraseSafeVectorProxy() noexcept
		: m_curTraverseIndex(0),
			m_isTraversing(com::False) { }

		void push_back(const T& value) noexcept
		{
			// For now we do not know what to do if push_back() is called while being traversed.
			_com_assert(static_cast<bool>(!m_isTraversing));
			m_data.push_back(value);
		}
		void pop_back() noexcept { m_data.pop_back(); }
		T& back() noexcept { return m_data.back(); }
		const T& back() const noexcept { return m_data.back(); }
		T& front() noexcept { return m_data.front(); }
		const T& front() const noexcept { return m_data.front(); }
		size_type size() const noexcept { return m_data.size(); }

		// Only allow constant iterators so that they won't be accepted by the erase() function 
		// As erase function takes non-constant iterator.
		const_iterator begin() const noexcept { return m_data.begin(); }
		const_iterator end() const noexcept { return m_data.end(); }
		const_iterator cbegin() const noexcept { return begin(); }
		const_iterator cend() const noexcept { return end(); }

		iterator indexToIterator(size_type index) noexcept
		{
			_com_assert(index <= m_data.size());
			return std::next(m_data.begin(), index);
		}
		
		com::Bool isValidIterator(iterator it) noexcept { return com::Bool { it != m_data.end() }; }
		com::Bool isValidIterator(const_iterator it) const noexcept { return com::Bool { it != m_data.cend() }; }

		iterator find(const T& value) noexcept { return std::find(m_data.begin(), m_data.end(), value); }
		const_iterator find(const T& value) const noexcept { return std::find(m_data.cbegin(), m_data.cend(), value); }
		com::Bool contains(const T& value) const noexcept { return isValidIterator(find(value)); }

		// Since range based loop would use const_iterator (as we have only provided that),
		// erase() can't be used inside a range-based loop - which is what we wanted!
		// So one must have to 'traverse' function to really traverse the vector and potentially be able to erase elements during traversal
		void erase(iterator pos) noexcept;
		// Only allowed to be called in the visitor callback of 'traverse' function
		// This erases the current element being traversed, and following this call no attempt should be made to access the reference to the current element
		void eraseCurrent() noexcept
		{
			_com_assert(static_cast<bool>(m_isTraversing));
			erase(indexToIterator(m_curTraverseIndex));
		}

		void find_erase(const T& value) noexcept
		{
			iterator it = find(value);
			if(isValidIterator(it))
				erase(it);
			else // This should never happen
				com_assert(COM_DESCRIPTION(false), "attempt to erase an element which doesn't really exist");
		}
		
		template<com::concepts::UnaryVisitor<T> Visitor>
		void traverse(Visitor visitor) noexcept;
	};

	template<typename T>
	void UnorderedEraseSafeVectorProxy<T>::erase(iterator pos) noexcept
	{
		if(m_isTraversing)
		{
			_com_assert(m_data.size() > 0);
			size_type eraseIndex = std::distance(m_data.begin(), pos);
			if(eraseIndex < m_curTraverseIndex)
			{
				m_data.erase(pos);
				--m_curTraverseIndex;
			}
			else if(eraseIndex >= m_curTraverseIndex)
			{
				*pos = std::move(m_data[m_data.size() - 1]);
				m_data.pop_back();
				if(eraseIndex == m_curTraverseIndex)
					--m_curTraverseIndex;
			}
		}
		else m_data.erase(pos);
	}

	template<typename T>
	template<com::concepts::UnaryVisitor<T> Visitor>
	void UnorderedEraseSafeVectorProxy<T>::traverse(Visitor visitor) noexcept
	{
		m_isTraversing = com::True;
		m_curTraverseIndex = 0;
		while(m_curTraverseIndex < m_data.size())
		{
			visitor(m_data[m_curTraverseIndex]);
			++m_curTraverseIndex;
		}
		m_isTraversing = com::False;
	}
}
#pragma once

#include <common/id_generator.h> // for generating unique ids for each subscription
#include <common/debug.h> // for debug_log_error
#include <common/assert.h> // for _com_assert

#include <functional> // for std::function
#include <unordered_map> // for std::unordered_map

namespace com
{
	template<typename PublisherType, typename... Args>
	class COMMON_API Event
	{
	public:
		typedef PublisherType* PublisherTypePtr;
		typedef id_generator_id_type_t SubscriptionID;
		typedef std::function<void(PublisherTypePtr, Args...)> EventHandler;
		static constexpr id_generator_id_type_t InvalidSubscriptionID = ID_GENERATOR_ID_TYPE_MAX;
	private:
		id_generator_t m_id_generator;
		PublisherTypePtr m_publisher;
		std::unordered_map<SubscriptionID, std::pair<EventHandler, bool>> m_handlers;
		std::vector<SubscriptionID> m_unsubscribeRequests;
		bool m_isPublishing;

		typename std::unordered_map<SubscriptionID, std::pair<EventHandler, bool>>::iterator
	 	getHandlerIt(SubscriptionID id)
		{
			_com_assert(id);
			auto it = m_handlers.find(id);
			if(it == m_handlers.end())
				debug_log_error("You're trying to access a subscription which you never subscribed to!");
			return it;
		}

		void unsubscribeImmediately(SubscriptionID id)
		{
			_com_assert(id);
			auto it = getHandlerIt(id);
			m_handlers.erase(it);
		}

	public:

		Event(PublisherTypePtr publisher) noexcept : m_id_generator(id_generator_create(0, NULL)), m_publisher(publisher), m_isPublishing(false) { }
		Event() noexcept : Event(NULL) { }

		void setPublisher(PublisherTypePtr ptr) noexcept { m_publisher = ptr; }
		PublisherTypePtr getPublisher() noexcept { return m_publisher; }

		SubscriptionID subscribe(EventHandler handler) noexcept
		{
			auto id = id_generator_get(&m_id_generator);
			m_handlers.insert({ id, { handler, true } });
			return id;
		}

		void unsubscribe(SubscriptionID id) noexcept
		{
			if(m_isPublishing)
			{
				m_unsubscribeRequests.push_back(id);
				return;
			}
			unsubscribeImmediately(id);
		}

		void deactivate(SubscriptionID id) noexcept
		{
			auto it = getHandlerIt(id);
			it->second.second = false;
		}

		void activate(SubscriptionID id) noexcept
		{
			auto it = getHandlerIt(id);
			it->second.second = true;
		}

		void publish(Args... args) noexcept
		{
			m_isPublishing = true;
			for(auto& pair : m_handlers)
			{
				// only invoke this handler if it is active
				if(pair.second.second)
					pair.second.first(m_publisher, args...);
			}
			m_isPublishing = false;
			if(m_unsubscribeRequests.size() > 0)
			{
				for(auto id : m_unsubscribeRequests)
					unsubscribeImmediately(id);
				m_unsubscribeRequests.clear();
			}
		}
	};
}
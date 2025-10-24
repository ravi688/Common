#pragma once

#include <common/id_generator.h> // for generating unique ids for each subscription
#include <common/debug.h> // for debug_log_error
#include <common/assert.h> // for _com_assert
#include <common/defines.hpp> // for com::KeyIteratable

#include <functional> // for std::function
#include <unordered_map> // for std::unordered_map
#include <type_traits> // for std::is_same_v

namespace com
{
	template<typename PublisherType, typename... Args>
	struct EventHandlerProto
	{
		using type = typename std::conditional<std::is_same<PublisherType, no_publish_ptr_t>::value, 
							std::function<void(Args...)>,
							std::function<void(PublisherType*, Args...)>
							>::type;
	};

	template<typename PublisherType, typename... Args>
	class COMMON_API Event
	{
	public:
		// This is used to distinguise com::OrderedEvent type from com::Event when passed as template type parameter
		static constexpr bool IsOrderedEventType = false;
		typedef PublisherType* PublisherTypePtr;
		typedef id_generator_id_type_t SubscriptionID;
		typedef typename EventHandlerProto<PublisherType, Args...>::type EventHandler;
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
			_com_assert(id != InvalidSubscriptionID);
			auto it = m_handlers.find(id);
			if(it == m_handlers.end())
				debug_log_error("You're trying to access a subscription which you never subscribed to!");
			return it;
		}

		void unsubscribeImmediately(SubscriptionID id)
		{
			_com_assert(id != InvalidSubscriptionID);
			auto it = getHandlerIt(id);
			m_handlers.erase(it);
		}

	public:


		template<typename T = PublisherType>
		requires(std::is_same_v<T, no_publish_ptr_t>)
		Event() noexcept : m_id_generator(id_generator_create(0, NULL)), m_isPublishing(false) { }

		template<typename T = PublisherType>
		requires(!std::is_same_v<T, no_publish_ptr_t>)
		Event(T* publisher) noexcept : m_id_generator(id_generator_create(0, NULL)), m_publisher(publisher), m_isPublishing(false) { }

		template<typename T = PublisherType>
		requires(std::is_same_v<T, no_publish_ptr_t>)
		Event(Event&& event) noexcept : m_id_generator(event.m_id_generator),
										m_handlers(std::move(event.m_handlers)),
										m_unsubscribeRequests(std::move(event.m_unsubscribeRequests)),
										m_isPublishing(event.m_isPublishing)
		{
			event.m_isPublishing = false;
		}

		template<typename T = PublisherType>
		requires(!std::is_same_v<T, no_publish_ptr_t>)
		Event(Event&& event) noexcept : m_id_generator(event.m_id_generator),
										m_publisher(event.m_publisher), 
										m_handlers(std::move(event.m_handlers)),
										m_unsubscribeRequests(std::move(event.m_unsubscribeRequests)),
										m_isPublishing(event.m_isPublishing)
		{
			event.m_publisher = NULL;
			event.m_isPublishing = false;
		}

		~Event() noexcept
		{
			id_generator_destroy(&m_id_generator);
		}

		// Description: Returns the number of subscriptions to this event
		// Params: None
		// Returns: an integer (unsigned)
		COM_NO_DISCARD auto size() const noexcept { return m_handlers.size(); }

		template<typename T = PublisherType>
		requires(!std::is_same_v<T, no_publish_ptr_t>)
		void setPublisher(T* ptr) noexcept { m_publisher = ptr; }

		template<typename T = PublisherType>
		requires(!std::is_same_v<T, no_publish_ptr_t>)
		T* getPublisher() noexcept { return m_publisher; }

		SubscriptionID subscribe(EventHandler handler) noexcept
		{
			auto id = id_generator_get(&m_id_generator);
			m_handlers.insert({ id, { std::move(handler), true } });
			return id;
		}

		void clear() noexcept
		{
			if(m_isPublishing)
			{
				com::KeyIteratable ids(m_handlers);
				for(auto& id : ids)
					unsubscribe(id);
			}
			else
			{
				id_generator_reset(&m_id_generator, 0);
				m_handlers.clear();
			}
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

		void publish(Args... args) noexcept requires(!std::is_same_v<PublisherType, no_publish_ptr_t>)
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

		void publish(Args... args) noexcept requires(std::is_same_v<PublisherType, no_publish_ptr_t>)
		{
			m_isPublishing = true;
			for(auto& pair : m_handlers)
			{
				// only invoke this handler if it is active
				if(pair.second.second)
					pair.second.first(args...);
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
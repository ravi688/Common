#include <common/id_generator.h> // for generating unique ids for each subscription
#include <common/debug.h> // for debug_log_error

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
	private:
		id_generator_t m_id_generator;
		PublisherTypePtr m_publisher;
		std::unordered_map<SubscriptionID, EventHandler> m_handlers;
		std::vector<SubscriptionID> m_unsubscribeRequests;
		bool m_isPublishing;

		void unsubscribeImmediately(SubscriptionID id)
		{
			auto it = m_handlers.find(id);
			if(it != m_handlers.end())
				m_handlers.erase(it);
			else
				debug_log_error("You're trying to unsubscribe a subscription which you never subscribed to!");
		}

	public:

		Event(PublisherTypePtr publisher) noexcept : m_id_generator(id_generator_create(0, NULL)), m_publisher(publisher), m_isPublishing(false) { }
		Event() noexcept : Event(NULL) { }

		void setPublisher(PublisherTypePtr ptr) noexcept { m_publisher = ptr; }
		PublisherTypePtr getPublisher() noexcept { return m_publisher; }

		SubscriptionID subscribe(EventHandler handler) noexcept
		{
			auto id = id_generator_get(&m_id_generator);
			m_handlers.insert({ id, handler });
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

		void publish(Args... args) noexcept
		{
			m_isPublishing = true;
			for(auto& pair : m_handlers)
				pair.second(m_publisher, args...);
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
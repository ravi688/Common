#pragma once

#include <common/id_generator.h> // for generating unique ids for each subscription
#include <common/debug.h> // for debug_log_error
#include <common/assert.h> // for _com_assert
#include <common/defines.hpp> // for com::KeyIteratable

#include <functional> // for std::function
#include <unordered_map> // for std::unordered_map
#include <map> // for std::multimap
#include <type_traits> // for std::is_same_v

namespace com
{
	template<typename PublisherType, typename... Args>
	struct OrderedEventHandlerProto
	{
		using type = typename std::conditional<std::is_same<PublisherType, no_publish_ptr_t>::value, 
							std::function<bool(Args...)>,
							std::function<bool(PublisherType*, Args...)>
							>::type;
	};

	template<typename PublisherType, typename KeyType, class Compare = std::less<KeyType>, typename... Args>
	class COMMON_API OrderedEvent
	{
	public:
		// This is used to distinguise com::OrderedEvent type from com::Event when passed as template type parameter
		static constexpr bool IsOrderedEventType = true;
		typedef PublisherType* PublisherTypePtr;
		typedef id_generator_id_type_t SubscriptionID;
		typedef typename OrderedEventHandlerProto<PublisherType, Args...>::type EventHandler;
		static constexpr id_generator_id_type_t InvalidSubscriptionID = ID_GENERATOR_ID_TYPE_MAX;
	private:
		struct EventHandlerData
		{
			EventHandler handler;
			bool isActive;
			bool isTempInActive;
			KeyType key;
		};
		struct KeyUpdateRequest
		{
			SubscriptionID id;
			KeyType newKey;
		};
	private:
		id_generator_t m_id_generator;
		PublisherTypePtr m_publisher;
		std::unordered_map<SubscriptionID, EventHandlerData> m_handlers;
		std::multimap<KeyType, EventHandlerData*, Compare> m_orderedMap;
		std::vector<SubscriptionID> m_unsubscribeRequests;
		std::vector<KeyUpdateRequest> m_keyUpdateRequests;
		SubscriptionID m_exclusiveAccessID;
		EventHandlerData* m_exclusiveHandlerData;
		bool m_isPublishing;

		typename std::unordered_map<SubscriptionID, EventHandlerData>::iterator
	 	getHandlerIt(SubscriptionID id)
		{
			_com_assert(id != InvalidSubscriptionID);
			auto it = m_handlers.find(id);
			if(it == m_handlers.end())
				debug_log_error("You're trying to access a subscription which you never subscribed to!");
			return it;
		}

		auto getOrderedMapIt(const EventHandlerData& data) noexcept
		{
			auto range = m_orderedMap.equal_range(data.key);
			auto foundIt = range.first;
			if(foundIt != range.second)
			{
				for(auto it = range.first; it != range.second; ++it)
					if(it->second == (&data))
						foundIt = it;
			}
			return foundIt;
		}

		void unsubscribeImmediately(SubscriptionID id)
		{
			_com_assert(id != InvalidSubscriptionID);
			auto it = getHandlerIt(id);
			auto it2 = getOrderedMapIt(it->second);
			m_orderedMap.erase(it2);
			m_handlers.erase(it);
		}

		void updateKeyImmediately(SubscriptionID id, const KeyType& newKey) noexcept
		{
			EventHandlerData& data = com::find_value(m_handlers, id);
			auto it = getOrderedMapIt(data);
			m_orderedMap.erase(it);
			data.key = newKey;
			m_orderedMap.insert({ newKey, &data });
		}

	public:


		template<typename T = PublisherType>
		requires(std::is_same_v<T, no_publish_ptr_t>)
		OrderedEvent() noexcept : m_id_generator(id_generator_create(0, NULL)), m_exclusiveAccessID(InvalidSubscriptionID), m_exclusiveHandlerData(com::null_pointer<EventHandlerData>()), m_isPublishing(false) { }

		template<typename T = PublisherType>
		requires(!std::is_same_v<T, no_publish_ptr_t>)
		OrderedEvent(T* publisher) noexcept : m_id_generator(id_generator_create(0, NULL)), m_publisher(publisher), m_exclusiveAccessID(InvalidSubscriptionID), m_exclusiveHandlerData(com::null_pointer<EventHandlerData>()), m_isPublishing(false) { }

		template<typename T = PublisherType>
		requires(std::is_same_v<T, no_publish_ptr_t>)
		OrderedEvent(OrderedEvent&& event) noexcept : m_id_generator(event.m_id_generator),
										m_handlers(std::move(event.m_handlers)),
										m_orderedMap(std::move(event.m_orderedMap)),
										m_unsubscribeRequests(std::move(event.m_unsubscribeRequests)),
										m_exclusiveAccessID(InvalidSubscriptionID),
										m_exclusiveHandlerData(com::null_pointer<EventHandlerData>()),
										m_isPublishing(event.m_isPublishing)
		{
			event.m_isPublishing = false;
		}

		template<typename T = PublisherType>
		requires(!std::is_same_v<T, no_publish_ptr_t>)
		OrderedEvent(OrderedEvent&& event) noexcept : m_id_generator(event.m_id_generator),
										m_publisher(event.m_publisher), 
										m_handlers(std::move(event.m_handlers)),
										m_orderedMap(std::move(event.m_orderedMap)),
										m_unsubscribeRequests(std::move(event.m_unsubscribeRequests)),
										m_exclusiveAccessID(InvalidSubscriptionID),
										m_exclusiveHandlerData(com::null_pointer<EventHandlerData>()),
										m_isPublishing(event.m_isPublishing)
		{
			event.m_publisher = NULL;
			event.m_isPublishing = false;
		}

		~OrderedEvent() noexcept
		{
			id_generator_destroy(&m_id_generator);
		}

		template<typename T = PublisherType>
		requires(!std::is_same_v<T, no_publish_ptr_t>)
		void setPublisher(T* ptr) noexcept { m_publisher = ptr; }

		template<typename T = PublisherType>
		requires(!std::is_same_v<T, no_publish_ptr_t>)
		T* getPublisher() noexcept { return m_publisher; }

		SubscriptionID subscribe(EventHandler handler, const KeyType& key) noexcept
		{
			auto id = id_generator_get(&m_id_generator);
			m_handlers.insert({ id, { std::move(handler), true, false, key } });
			EventHandlerData& data = com::find_value(m_handlers, id);
			m_orderedMap.insert({ key, &data });
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
				m_orderedMap.clear();
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
			it->second.isActive = false;
		}

		void tempDeactivate(SubscriptionID id) noexcept
		{
			auto it = getHandlerIt(id);
			it->second.isTempInActive = true;
		}

		void activate(SubscriptionID id) noexcept
		{
			auto it = getHandlerIt(id);
			it->second.isActive = true;
		}

		void grabExclusiveAccess(SubscriptionID id) noexcept
		{
			if(m_exclusiveAccessID != InvalidSubscriptionID)
			{
				debug_log_fetal_error("You're trying to grab exclusive access twice (without releasing it first), which is not possible");
				return;
			}
			m_exclusiveAccessID = id;
			m_exclusiveHandlerData = &getHandlerIt(id)->second;
		}

		void releaseExclusiveAccess(SubscriptionID id) noexcept
		{
			if(m_exclusiveAccessID != id)
			{
				debug_log_error("You released exclusive access with a different subscription id than what was used for the grabbing");
				return;
			}
			m_exclusiveAccessID = InvalidSubscriptionID;
			m_exclusiveHandlerData = com::null_pointer<EventHandlerData>();
		}

		SubscriptionID getExclusiveAccessID() const noexcept { return m_exclusiveAccessID; }

		void updateKey(SubscriptionID id, const KeyType& newKey) noexcept
		{
			if(m_isPublishing)
			{
				m_keyUpdateRequests.push_back({ id, newKey });
				return;
			}
			updateKeyImmediately(id, newKey);
		}

		void publish(Args... args) noexcept
		{
			m_isPublishing = true;
			if(m_exclusiveHandlerData)
			{
				if constexpr (std::is_same_v<PublisherType, no_publish_ptr_t>)
					m_exclusiveHandlerData->handler(args...);
				else
					m_exclusiveHandlerData->handler(m_publisher, args...);
			}
			else
			{
				for(auto it = m_orderedMap.begin(); it != m_orderedMap.end(); ++it)
				{
					auto& pair = *it;
					// only invoke this handler if it is active
					if(!pair.second->isTempInActive && pair.second->isActive)
					{
						bool isStop = pair.second->handler(m_publisher, args...);
						if(isStop)
						{
							++it;
							while((it != m_orderedMap.end()) && (it->first == pair.first))
							{
								if(!pair.second->isTempInActive && it->second->isActive)
								{
									if constexpr (std::is_same_v<PublisherType, no_publish_ptr_t>)
										it->second->handler(args...);
									else
										it->second->handler(m_publisher, args...);
								}
								else if(pair.second->isTempInActive)
									pair.second->isTempInActive = false;
								++it;
							}
							break;
						}
					}
					else if(pair.second->isTempInActive)
						pair.second->isTempInActive = false;
				}
			}
			m_isPublishing = false;
			if(m_unsubscribeRequests.size() > 0)
			{
				for(auto id : m_unsubscribeRequests)
					unsubscribeImmediately(id);
				m_unsubscribeRequests.clear();
			}
			if(m_keyUpdateRequests.size() > 0)
			{
				for(auto request : m_keyUpdateRequests)
					updateKeyImmediately(request.id, request.newKey);
				m_keyUpdateRequests.clear();
			}
		}

		__attribute__((noinline))
		void dump() const noexcept
		{
			std::cout << "Ordered Event (Dump): " << "\n";
			for(auto& pair : m_handlers)
				std::cout << pair.first << " -> " << pair.second.key << std::endl;
			std::cout << "----\n";
			for(auto& pair : m_orderedMap)
			{
				std::cout << pair.first << ": ";
				for(auto& pair1 : m_handlers)
					if(pair1.second.key == pair.first)
						std::cout << pair1.first << " ";
				std::cout << "\n";
			}
			std::cout << std::endl;
		}
	};

}
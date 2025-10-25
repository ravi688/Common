// Test framework header inclusions
#include <catch2/catch_test_macros.hpp>

// The headers we are primarily interested in
#include <common/Event.hpp>

// Standard Headers
#include <vector>
#include <cstdlib> // for std::rand() and std::srand()
#include <ctime> // for std::time()

TEST_CASE("Simple tests on Event<>", "[Event-Simple]" )
{
    using TestEvent = com::Event<com::no_publish_ptr_t>;
    TestEvent event;
    
    SECTION("clear() on empty Event")
    {
        REQUIRE(event.size() == 0);
        event.clear();
        REQUIRE(event.size() == 0);
    }

    SECTION("subscribe() should increase the value returned by size()")
    {
        event.subscribe([]() noexcept { return false; });
        REQUIRE(event.size() == 1);
        
        for(u32 i = 0; i < 200; ++i)
            event.subscribe([]() noexcept { return false; });
        REQUIRE(event.size() == 201);

        SECTION("clear() should make size() return zero")
        {
            event.clear();
            REQUIRE(event.size() == 0);
        }
    }

    SECTION("unsubscribe() should decrease the value returned by size()")
    {
        std::vector<TestEvent::SubscriptionID> ids;
        for(u32 i = 0; i < 200; ++i)
            ids.push_back(event.subscribe([]() noexcept { }));
        REQUIRE(ids.size() == 200);
        for(u32 i = 50, j = 200; i < 200; ++i)
        {
            event.unsubscribe(ids[i]);
            --j;
            REQUIRE(event.size() == j);
        }
        REQUIRE(event.size() == 50);
        for(u32 i = 0; i < 50; ++i)
            event.unsubscribe(ids[i]);
        REQUIRE(event.size() == 0);
    }

    SECTION("Deactivation and Activation of handlers")
    {
        std::size_t numCalls = 0;
        std::vector<TestEvent::SubscriptionID> ids;
        for(u32 i = 0; i < 200; ++i)
            ids.push_back(event.subscribe([&numCalls]() noexcept { ++numCalls; }));
        event.publish();
        REQUIRE(numCalls == 200);

        SECTION("deactivating few of them should decrease the number of handler invocations")
        {
            numCalls = 0;
            for(u32 i = 13; i < 25; ++i)
                event.deactivate(ids[i]);
            event.publish();
            REQUIRE(numCalls == 188);
            SECTION("reactivating the deactivated handlers should restore the original number of handler invocations")
            {
                numCalls = 0;
                for(u32 i = 13; i < 25; ++i)
                    event.activate(ids[i]);
                event.publish();
                REQUIRE(numCalls == 200);
            }
        }
    }
}

TEST_CASE("Complex tests on Event<>", "[Event-Complex]" )
{
    using TestEvent = com::Event<void, int>;
    TestEvent event(static_cast<void*>(nullptr));

    SECTION("A handler should deprive the handlers with keys smaller than its (larger) key")
    {
        std::size_t numCalls = 0;
        std::size_t numCalls2 = 0;
        int64_t globalValue = 0;
        int64_t globalValue2 = 0;
        for(u32 i = 0; i < 100; ++i)
            event.subscribe([&numCalls, &globalValue]([[maybe_unused]] void*, [[maybe_unused]] int value) noexcept { ++numCalls; globalValue += value; });
        std::vector<int> keys;
        for(u32 i = 0; i < 10; ++i)
            keys.push_back(event.subscribe([&numCalls2, &globalValue2]([[maybe_unused]] void*, [[maybe_unused]] int value) noexcept { ++numCalls2; globalValue2 += value; }));
        for(u32 i = 0; i < 100; ++i)
            event.subscribe([&numCalls, &globalValue]([[maybe_unused]] void*, [[maybe_unused]] int value) noexcept { ++numCalls; globalValue += value; });
        int publishValue = 23;

        event.publish(publishValue);
        REQUIRE(numCalls == 200);
        REQUIRE(numCalls2 == 10);
        REQUIRE(globalValue == (numCalls * publishValue));
        REQUIRE(globalValue2 == (numCalls2 * publishValue));

        SECTION("move constructor")
        {
            numCalls = 0;
            numCalls2 = 0;
            globalValue = 0;
            globalValue2 = 0;
            auto event2 = std::move(event);
            event2.publish(publishValue);
            REQUIRE(numCalls == 200);
            REQUIRE(numCalls2 == 10);
            REQUIRE(globalValue == (numCalls * publishValue));
            REQUIRE(globalValue2 == (numCalls2 * publishValue));
        }

        SECTION("deactivating the handlers which return true (i.e. stop others) should increase the number of invoked handlers")
        {
            numCalls = 0;
            numCalls2 = 0;
            globalValue = 0;
            globalValue2 = 0;
            for(const auto& key : keys)
                event.deactivate(key);
    
            publishValue = 56;
            event.publish(publishValue);
            REQUIRE(numCalls == 200);
            REQUIRE(numCalls2 == 0);
            REQUIRE(globalValue == (numCalls * publishValue));
            REQUIRE(globalValue2 == (numCalls2 * publishValue));
        }

        SECTION("activating the handlers again")
        {
            numCalls = 0;
            numCalls2 = 0;
            globalValue = 0;
            globalValue2 = 0;
            for(const auto& key : keys)
                event.activate(key);
    
            publishValue = 34;
            event.publish(publishValue);
            REQUIRE(numCalls == 200);
            REQUIRE(numCalls2 == 10);
            REQUIRE(globalValue == (numCalls * publishValue));
            REQUIRE(globalValue2 == (numCalls2 * publishValue));
        }
        
        SECTION("Unsubscribing the handlers")
        {
            numCalls = 0;
            numCalls2 = 0;
            globalValue = 0;
            globalValue2 = 0;
            for(const auto& key : keys)
                event.unsubscribe(key);
    
            publishValue = 67;
            event.publish(publishValue);
            REQUIRE(numCalls == 200);
            REQUIRE(numCalls2 == 0);
            REQUIRE(globalValue == (numCalls * publishValue));
            REQUIRE(globalValue2 == (numCalls2 * publishValue));
        }
    }
}


class UDT
{
private:
    std::size_t& m_counter;

public:
    UDT(std::size_t& counter) : m_counter(counter) { ++m_counter; }

    UDT(const UDT& udt) : m_counter(udt.m_counter)
    {
        ++m_counter;
    }
};

TEST_CASE("Argument type")
{
    com::Event<com::no_publish_ptr_t, UDT*> myEvent2;
    myEvent2.subscribe([](const UDT*) { });
    myEvent2.publish(nullptr);

    com::Event<com::no_publish_ptr_t, UDT> myEvent;
    myEvent.subscribe([](UDT udt) { });

    std::size_t counter = 0;
    UDT udt(counter);
    REQUIRE(counter == 1);
    myEvent.publish(udt);

    // Pass by value causes 2 copies
    // So total constructor calls 1 + 2 = 3
    REQUIRE(counter == 3);

    myEvent.clear();
    myEvent.subscribe([](const UDT& udt) { });
    counter = 0;
    UDT udt2(counter);
    myEvent.publish(udt2);

    // Pass by reference causes only 1 copy
    // So total constructor calls 1 + 1 = 2
    REQUIRE(counter == 2);
}

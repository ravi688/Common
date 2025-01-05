// Test framework header inclusions
#include <catch2/catch_test_macros.hpp>

// The headers we are primarily interested in
#include <common/OrderedEvent.hpp>

// Standard Headers
#include <vector>
#include <cstdlib> // for std::rand() and std::srand()
#include <ctime> // for std::time()


TEST_CASE("Simple tests on OrderedEvent<>", "[OrderedEvent-Simple]" )
{
    using TestEvent = com::OrderedEvent<com::no_publish_ptr_t, int>;
    TestEvent event;
    
    SECTION("clear() on empty OrderedEvent")
    {
        REQUIRE(event.size() == 0);
        event.clear();
        REQUIRE(event.size() == 0);
    }

    SECTION("subscribe() should increase the value returned by size()")
    {
        event.subscribe([]() noexcept { return false; }, 342);
        REQUIRE(event.size() == 1);
        
        for(u32 i = 0; i < 200; ++i)
            event.subscribe([]() noexcept { return false; }, 342 + i);
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
            ids.push_back(event.subscribe([]() noexcept { return false; }, 342 + i));
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

    SECTION("Handlers with smallest keys should be called earlier")
    {
        std::srand(std::time(0));
        std::vector<int> orderCheck;
        std::vector<TestEvent::SubscriptionID> ids;
        std::vector<int> keys;
        for(u32 i = 0; i < 200; ++i)
        {
            auto key = std::rand();
            keys.push_back(key);
            ids.push_back(event.subscribe([key, &orderCheck]() noexcept { orderCheck.push_back(key); return false; }, key));
        }
        event.publish();
        REQUIRE(orderCheck.size() == 200);
        for(u32 i = 0; i < (orderCheck.size() - 1); ++i)
            REQUIRE(orderCheck[i] <= orderCheck[i + 1]);

        SECTION("deactivating few of them should decrease the number of handler invocations")
        {
            orderCheck.clear();
            for(u32 i = 13; i < 25; ++i)
                event.deactivate(ids[i]);
            event.publish();
            REQUIRE(orderCheck.size() == 188);
            for(u32 i = 0; i < (orderCheck.size() - 1); ++i)
                REQUIRE(orderCheck[i] <= orderCheck[i + 1]);
            SECTION("reactivating the deactivated handlers should restore the original number of handler invocations")
            {
                orderCheck.clear();
                for(u32 i = 13; i < 25; ++i)
                    event.activate(ids[i]);
                event.publish();
                REQUIRE(orderCheck.size() == 200);
                for(u32 i = 0; i < (orderCheck.size() - 1); ++i)
                    REQUIRE(orderCheck[i] <= orderCheck[i + 1]);
            }
        }

        SECTION("temporarily deactivating all of them should cause no invocations")
        {
            orderCheck.clear();
            for(u32 i = 0; i < event.size(); ++i)
                event.tempDeactivate(ids[i]);
            event.publish();
            REQUIRE(orderCheck.size() == 0);
            SECTION("After publishing one time all the temporarily deactivated handlers should have now been reactivated")
            {
                event.publish();
                REQUIRE(orderCheck.size() == 200);
                for(u32 i = 0; i < (orderCheck.size() - 1); ++i)
                    REQUIRE(orderCheck[i] <= orderCheck[i + 1]);
            }
        }

        SECTION("exclusive access should cause only one handler to be invoked")
        {
            REQUIRE(event.getExclusiveAccessID() == TestEvent::InvalidSubscriptionID);
            orderCheck.clear();
            event.grabExclusiveAccess(ids[0]);
            REQUIRE(event.getExclusiveAccessID() == ids[0]);
            event.publish();
            REQUIRE(orderCheck.size() == 1);
            REQUIRE(orderCheck[0] == keys[0]);
            event.releaseExclusiveAccess(ids[0]);
            REQUIRE(event.getExclusiveAccessID() == TestEvent::InvalidSubscriptionID);
        }
    }
}

TEST_CASE("Complex tests on OrderedEvent<>", "[OrderedEvent-Complex]" )
{
    using TestEvent = com::OrderedEvent<void, int>;
    TestEvent event(static_cast<void*>(nullptr));

    SECTION("A handler should deprive the handlers with keys smaller than its (larger) key")
    {
        // Random seed
        std::srand(std::time(0));
        std::vector<int> orderCheck;
        for(u32 i = 0; i < 100; ++i)
        {
            auto key = std::rand() % 200;
            event.subscribe([key, &orderCheck]([[maybe_unused]] void* _) noexcept { orderCheck.push_back(key); return false; }, key);
        }
        std::vector<int> orderCheck2;
        std::vector<int> keys;
        for(u32 i = 0; i < 10; ++i)
            keys.push_back(event.subscribe([&orderCheck2]([[maybe_unused]] void* _) noexcept { orderCheck2.push_back(300); return true; }, 300));
        for(u32 i = 0; i < 100; ++i)
        {
            auto key = 400 + std::rand() % 200;
            event.subscribe([key, &orderCheck]([[maybe_unused]] void* _) noexcept { orderCheck.push_back(key); return false; }, key);
        }
        event.publish();
        REQUIRE(orderCheck.size() == 100);
        for(u32 i = 0; i < (orderCheck.size() - 1); ++i)
            REQUIRE(orderCheck[i] <= orderCheck[i + 1]);
        REQUIRE(orderCheck2.size() == 10);

        SECTION("deactivating the handlers which return true (i.e. stop others) should increase the number of invoked handlers")
        {
            orderCheck.clear();
            orderCheck2.clear();
            for(const auto& key : keys)
                event.deactivate(key);
    
            event.publish();
            REQUIRE(orderCheck.size() == 200);
            for(u32 i = 0; i < (orderCheck.size() - 1); ++i)
                REQUIRE(orderCheck[i] <= orderCheck[i + 1]);
            REQUIRE(orderCheck2.size() == 0);
        }
        
        SECTION("Unsubscribing the handlers which return true (i.e. stop others) should increase the number of invoked handlers")
        {
            orderCheck.clear();
            orderCheck2.clear();
            for(const auto& key : keys)
                event.unsubscribe(key);
    
            event.publish();
            REQUIRE(orderCheck.size() == 200);
            for(u32 i = 0; i < (orderCheck.size() - 1); ++i)
                REQUIRE(orderCheck[i] <= orderCheck[i + 1]);
            REQUIRE(orderCheck2.size() == 0);
        }
    }
}
// Test framework header inclusions
#include <catch2/catch_test_macros.hpp>

// The headers we are primarily interested in
#include <common/DynamicPool.hpp>

// Standard Headers
#include <vector>

TEST_CASE("Simple tests on DynamicPool<>", "[DynamicPool-Simple]" )
{
    com::DynamicPool<int> pool([]()
    { 
        static int counter = 0;
        return counter++;
    });

    SECTION("Initial parameters check")
    {
        REQUIRE(pool.activeCount() == 0);
        REQUIRE(pool.size() == 0);
        REQUIRE(pool.begin() == pool.end());
        auto itable = pool.getActives();
        REQUIRE(itable.begin() == itable.end());
    }

    SECTION("Modified state parameters check")
    {
        auto v1 = pool.get();
        REQUIRE(pool.activeCount() == 1);
        REQUIRE(pool.size() == 1);
        REQUIRE(pool.begin() != pool.end());
        REQUIRE(std::next(pool.begin(), 1) == pool.end());
        REQUIRE(std::next(pool.getActives().begin(), 1) == pool.getActives().end());

        REQUIRE(v1 == 0);
        auto v2 = pool.get();
        auto v3 = pool.get();
        REQUIRE(pool.activeCount() == 3);
        REQUIRE(pool.size() == 3);
        REQUIRE(pool.begin() != pool.end());
        REQUIRE(std::next(pool.begin(), 3) == pool.end());
        REQUIRE(std::next(pool.getActives().begin(), 3) == pool.getActives().end());

        pool.put(v1);
        REQUIRE(pool.activeCount() == 2);
        REQUIRE(pool.size() == 3);
        REQUIRE(pool.begin() != pool.end());
        REQUIRE(std::next(pool.begin(), 3) == pool.end());
        REQUIRE(std::next(pool.getActives().begin(), 2) == pool.getActives().end());

        auto k1 = *pool.getActives().begin();
        auto k2 = *std::next(pool.getActives().begin(), 1);
        bool isK1exists = (k1 == v2) || (k1 == v3);
        REQUIRE(isK1exists);
        if(k1 == v2)
            REQUIRE(k2 == v3);
        else
            REQUIRE(k2 == v2);

        pool.clear();
        REQUIRE(pool.activeCount() == 0);
        REQUIRE(pool.size() == 0);
        REQUIRE(pool.begin() == pool.end());
        auto itable = pool.getActives();
        REQUIRE(itable.begin() == itable.end());

        v1 = pool.get();
        v2 = pool.get();
        v3 = pool.get();
        v3 = pool.get();
        REQUIRE(pool.activeCount() == 4);
        REQUIRE(pool.size() == 4);
        REQUIRE(pool.begin() != pool.end());
        REQUIRE(std::next(pool.begin(), 4) == pool.end());
        REQUIRE(std::next(pool.getActives().begin(), 4) == pool.getActives().end());

        pool.reclaim();
        REQUIRE(pool.activeCount() == 0);
        REQUIRE(pool.size() == 4);
        REQUIRE(pool.begin() != pool.end());
        REQUIRE(std::next(pool.begin(), 4) == pool.end());
        REQUIRE(std::next(pool.getActives().begin(), 0) == pool.getActives().end());

        pool.reserve(10);
        REQUIRE(pool.activeCount() == 0);
        REQUIRE(pool.size() == 10);
        REQUIRE(pool.begin() != pool.end());
        REQUIRE(std::next(pool.begin(), 10) == pool.end());
        REQUIRE(std::next(pool.getActives().begin(), 0) == pool.getActives().end());

        std::vector<int> v;
        for(int i = 0; i < 6; ++i)
            v.push_back(pool.get());

        REQUIRE(pool.activeCount() == 6);
        REQUIRE(pool.size() == 10);
        REQUIRE(pool.begin() != pool.end());
        REQUIRE(std::next(pool.begin(), 10) == pool.end());
        REQUIRE(std::next(pool.getActives().begin(), 6) == pool.getActives().end());

        for(int i = 0; i < 6; ++i)
            pool.put(v[i]);

        REQUIRE(pool.activeCount() == 0);
        REQUIRE(pool.size() == 10);
        REQUIRE(pool.begin() != pool.end());
        REQUIRE(std::next(pool.begin(), 10) == pool.end());
        REQUIRE(std::next(pool.getActives().begin(), 0) == pool.getActives().end());
    }
}

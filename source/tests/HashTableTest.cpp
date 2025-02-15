#include <catch2/catch_test_macros.hpp>
#include <common/hash_table.h>
#include <common/defines.hpp> // for com::size_t
#include <algorithm> // for std::next_permutation()
#include <numeric> // for std::iota()

TEST_CASE( "Hash Table", "[hash_table]" ) {
    SECTION("Create and Destroy with zero capacity")
    {
        hash_table_t table = hash_table_create(s32, float, 0, 1, s32_equal_to, s32_hash, NULL);
        REQUIRE(hash_table_get_count(&table) == 0);
        REQUIRE(hash_table_get_bucket_count(&table) == 1);
        hash_table_free(&table);
    }
    SECTION("Create and Destroy with non-zero capacity")
    {
        hash_table_t table = hash_table_create(s32, float, 500, 20, s32_equal_to, s32_hash, NULL);
        REQUIRE(hash_table_get_count(&table) == 0);
        REQUIRE(hash_table_get_bucket_count(&table) == 20);
        hash_table_free(&table);
    }
    hash_table_t table = hash_table_create(s32, float, 0, 10, s32_equal_to, s32_hash, NULL);
    SECTION("Add with initially zero capacity")
    {
        s32 key = 34;
        f32 value = 23.0f;
        hash_table_add(&table, &key, &value);
        REQUIRE(hash_table_get_count(&table) == 1);
        SECTION("Get After first Add")
        {
            f32 _value = DREF_TO(f32, hash_table_get_value(&table, &key));
            REQUIRE(_value == 23.0f);
        }
        SECTION("Remove After first Add")
        {
            hash_table_remove(&table, &key);
            REQUIRE(hash_table_get_count(&table) == 0);
            void* value_ptr = hash_table_get_value(&table, &key);
            REQUIRE(value_ptr == nullptr);
        }
    }
    SECTION("Add and Get Permutations")
    {
        std::vector<u32> indices(234);
        // NOTE: We can't use std::ranges::iota() here as it is only available in C++23
        std::iota(indices.begin(), indices.end(), 0);
        COM_REPEAT(123)
        {
            for(u32 i = 0; i < 234; ++i)
            {
                s32 key = i;
                f32 value = (i + 300) / 234.0f;
                hash_table_add(&table, &key, &value);
                REQUIRE(hash_table_get_count(&table) == (i + 1));
            }
            // Check for the values if they are correctly registered in the hash table or not
            // We need to permute the order of queries to increase the quality of this test.
            COM_REPEAT(123)
            {
                for(const auto& i : indices)
                {
                    u32 key = i;
                    f32 _value = DREF_TO(f32, hash_table_get_value(&table, &key));
                    f32 value = (i + 300) / 234.0f;
                    REQUIRE(_value == value);
                    REQUIRE(hash_table_contains(&table, &key) == true);
                }
                std::ranges::next_permutation(indices);
            }
            hash_table_clear(&table);
            // Now every value query must return nullptr as the key value pairs would no longer exist
            REQUIRE(hash_table_get_count(&table) == 0);
            COM_REPEAT(123)
            {
                for(const auto& i : indices)
                {
                    u32 key = i;
                    void* value = hash_table_get_value(&table, &key);
                    REQUIRE(value == nullptr);
                    REQUIRE(hash_table_contains(&table, &key) == false);
                }
            }
        }
    }
    hash_table_free(&table);
}
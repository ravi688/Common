#include <catch2/catch_test_macros.hpp>
#include <common/defines.hpp>

TEST_CASE( "Type Traits", "[type-traits]" ) {
    SECTION("com::remove_pointer_and_reference")
    {
        REQUIRE(std::is_same_v<typename com::remove_pointer_and_reference<int*>::type, int> == true);
        REQUIRE(std::is_same_v<typename com::remove_pointer_and_reference<int>::type, int> == true);
    }
    SECTION("com::is_refers_to_const_value")
    {
        REQUIRE(com::is_refers_to_const_value<int&>::value == false);
        REQUIRE(com::is_refers_to_const_value<const int&>::value == true);
        REQUIRE(com::is_refers_to_const_value<int>::value == false);
        REQUIRE(com::is_refers_to_const_value<const int>::value == true);
        REQUIRE(com::is_refers_to_const_value<int* const>::value == false);
        REQUIRE(com::is_refers_to_const_value<const int* const>::value == true);
    }
    SECTION("com::is_same_nocv")
    {
        REQUIRE(com::is_same_nocv<const int, int>::value == true);
        REQUIRE(com::is_same_nocv<int, int>::value == true);
        REQUIRE(com::is_same_nocv<const float, int>::value == false);
        REQUIRE(com::is_same_nocv<volatile int, int>::value == true);
    }
}

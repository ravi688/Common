#include <catch2/catch_test_macros.hpp>

#include <common/StringUtility.hpp>

TEST_CASE( "String Joining", "[com::string_join]" ) {
    REQUIRE( com::string_join("Hello", " World") == "Hello World" );
    REQUIRE( com::string_join("", " World") == " World" );
    REQUIRE( com::string_join("Hello ", "Another ", "World") == "Hello Another World" );
    REQUIRE( com::string_join(std::string { "Hello "}, "Another ", std::string_view { "World" }) == "Hello Another World" );
    char myStr[] = "Hello ";
    REQUIRE( com::string_join(myStr, "Another ", std::string_view { "World" }) == "Hello Another World");
}

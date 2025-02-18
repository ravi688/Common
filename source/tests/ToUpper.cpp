#include <catch2/catch_test_macros.hpp>
#include <common/defines.hpp>
#include <string>
#include <string_view>

TEST_CASE( "com::to_upper", "[to-upper-string]" ) {
    SECTION("Const strings")
    {
        const std::string str { "Hello World" };
        auto result = com::to_upper(str);
        REQUIRE(result == "HELLO WORLD");
        REQUIRE(std::is_same_v<decltype(result), std::string> == true);
 
        std::string_view strv { "Hello World" };
        auto result2 = com::to_upper(str);
        REQUIRE(result2 == "HELLO WORLD");
        REQUIRE(std::is_same_v<decltype(result2), std::string> == true);
    }
    SECTION("non-Const strings")
    {
        std::string str { "Hello World" };
        // NOTE: auto result = com::to_upper(str), would create a copy of std::string&, 
        // thus the decltype(result) would evaluate to std::string not std::string&
        // To preserve the reference, use auto&
        auto& result = com::to_upper(str);
        REQUIRE(result == "HELLO WORLD");
        REQUIRE(std::is_same_v<decltype(result), std::string&> == true);
    }
    SECTION("Literal strings")
    {
        REQUIRE(com::to_upper("Hello World") == "HELLO WORLD");
    }
}
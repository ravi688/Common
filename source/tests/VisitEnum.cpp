#include <catch2/catch_test_macros.hpp>
#include <common/defines.hpp>

#include <vector> // for std::vector<>

enum class MyEnum
{
    Value1,
    Value2,
    Value3,
    Value4,
    Value5,
    Value6,
    Value7,
    MAX
};

TEST_CASE( "com::VisitEnum", "[com::VisitEnum]" )
{
    std::vector<MyEnum> values;
    com::VisitEnum<MyEnum>([&values](MyEnum value) { values.push_back(value); }, MyEnum::MAX);

    REQUIRE(values.size() == static_cast<std::size_t>(com::to_underlying(MyEnum::MAX)));
    
    std::vector<MyEnum> refValues = { MyEnum::Value1, MyEnum::Value2, MyEnum::Value3, MyEnum::Value4, MyEnum::Value5, MyEnum::Value6, MyEnum::Value7 };

    REQUIRE(values == refValues);

    values.clear();

    com::VisitEnum<MyEnum>([&values](MyEnum value) { values.push_back(value); }, MyEnum::Value1);

    REQUIRE(values.size() == 0);
}

#include <catch2/catch_test_macros.hpp>

#include <common/BitCastBuffer.hpp>

// Should pass the BufferLike constraint
class DummyBuffer1
{
private:
    u32 m_value;
public:
    const char* data() const { return reinterpret_cast<const char*>(&m_value); }
    u32 size() const { return sizeof(m_value); }
};

static constexpr std::string gExampleString = "Hello World";
static constexpr std::size_t gExampleStringLength = gExampleString.size();

class Data
{
private:
    char _data[gExampleStringLength] = { };

public:
    char* c_str() { return _data; }
};

// Should fail the BufferLike constraint as it returns signed integer when size() is called.
class DummyBuffer2
{
public:
    const char* data() const { return nullptr; }
    s32 size() const { return 0; }
};

enum class TestEnum : u8
{
    EnumValue1,
    EnumValue2,
    EnumValue3,
    EnumValue4,
};

#pragma pack(push, 1)
struct PackedDataBuffer
{
    u32 value1 = 45;
    u8 value2 = 43;
    u16 value3 = 87;
    u8 value4 = 56;
    TestEnum value5 = TestEnum::EnumValue2;

    bool operator==(const PackedDataBuffer& buf) const noexcept
    {
        return buf.value1 == value1 && buf.value2 == value2 && buf.value3 == value3 && buf.value4 == value4 && buf.value5 == value5;   
    }

    const u8* data() const { return reinterpret_cast<const u8*>(this); }
    std::size_t size() const { return sizeof(PackedDataBuffer); }
};
#pragma pack(pop)

TEST_CASE( "BitCastBuffer", "[BitCastBuffer]" ) {

    REQUIRE( com::concepts::BufferLike<std::string> == true );
    REQUIRE( com::concepts::BufferLike<std::vector<char>> == true );
    REQUIRE( com::concepts::BufferLike<std::span<u8>> == true );
    REQUIRE( com::concepts::BufferLike<std::span<u8>> == true );
    REQUIRE( com::concepts::BufferLike<std::span<u32>> == false );
    REQUIRE( com::concepts::BufferLike<std::vector<std::vector<u8>>> == false);

    REQUIRE( com::concepts::BufferLike<DummyBuffer1> == true );
    REQUIRE( com::concepts::BufferLike<DummyBuffer2> == false );

    REQUIRE( com::BitCastBuffer<u32>(DummyBuffer1{ }).has_value() == true );
    REQUIRE( com::BitCastBuffer<u8>(DummyBuffer1 { }).has_value() == false );

    REQUIRE( com::BitCastBuffer<Data>(gExampleString).has_value() == true );
    auto str =  std::string_view { com::BitCastBuffer<Data>(gExampleString).value().c_str(), gExampleStringLength };
    REQUIRE( str == gExampleString );

    u32 value = 45;
    auto* valuePtr = reinterpret_cast<char*>(&value);
    REQUIRE( com::BitCastBuffer<u32>(std::string { valuePtr, valuePtr + sizeof(value) }).has_value() == true );
    REQUIRE( com::BitCastBuffer<u32>(std::string { valuePtr, valuePtr + sizeof(value) }).value() == value );

    TestEnum enumValue = TestEnum::EnumValue3;
    auto* valuePtr2 = reinterpret_cast<char*>(&enumValue);
    REQUIRE( com::BitCastBuffer<TestEnum>(std::string { valuePtr2, valuePtr2 + sizeof(enumValue) }).has_value() == true );
    REQUIRE( com::BitCastBuffer<TestEnum>(std::string { valuePtr2, valuePtr2 + sizeof(enumValue) }).value() == enumValue );

    auto packedResult = com::BitCastBufferPacked<u32, u8, u16, u8, TestEnum>(PackedDataBuffer { });
    REQUIRE( packedResult.has_value() == true );
    auto [value1, value2, value3, value4, value5] = *packedResult;
    REQUIRE( PackedDataBuffer { value1, value2, value3, value4, value5 } == PackedDataBuffer { } );
}


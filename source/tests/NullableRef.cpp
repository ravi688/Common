#include <catch2/catch_test_macros.hpp>

#include <common/NullableRef.hpp>

TEST_CASE( "NullableRef", "[NullableRef]" ) {

    REQUIRE( com::NullableRef<int>{ }.has_value() == false );
    REQUIRE( static_cast<bool>(com::NullableRef<int>{ }) == false );

    int value = 34;
    com::NullableRef<int> ref = value;
    REQUIRE( ref.has_value() == true );
    REQUIRE( static_cast<bool>(ref) == true );

    REQUIRE( *ref == value );
    REQUIRE( ref.value() == value );

    ref = 100;
    REQUIRE( ref.value() == 100 );
    REQUIRE( *ref == 100 );

    com::NullableRef<int> ref2 = ref;
    REQUIRE( ref2.value() == ref.value() );
}


#include <catch2/catch_test_macros.hpp>

#include <common/id_generator.h>

TEST_CASE( "id_generator_t", "[id_generator_t]" ) {

    id_generator_t idgen = id_generator_create(0, NULL);
    auto id = id_generator_get(&idgen);
    REQUIRE( id_generator_is_returnable(&idgen, id) == true );
    id_generator_return(&idgen, id);
    REQUIRE( id_generator_is_returnable(&idgen, id) == false );
    id = id_generator_get(&idgen);
    auto id2 = id_generator_get(&idgen);
    REQUIRE( id != id2 );
    REQUIRE( id_generator_is_returnable(&idgen, id) == true );
    REQUIRE( id_generator_is_returnable(&idgen, id2) == true );
    id_generator_reset(&idgen, 0);
    REQUIRE( id_generator_is_returnable(&idgen, id) == false );
    REQUIRE( id_generator_is_returnable(&idgen, id2) == false );
    id = id_generator_get(&idgen);
    id2 = id_generator_get(&idgen);
    REQUIRE( id != id2 );
    id_generator_return(&idgen, id2);
    REQUIRE( id_generator_is_returnable(&idgen, id) == true );
    REQUIRE( id_generator_is_returnable(&idgen, id2) == false );
    id_generator_return(&idgen, id);
    REQUIRE( id_generator_is_returnable(&idgen, id) == false );
    REQUIRE( id_generator_is_returnable(&idgen, id2) == false );
    id_generator_destroy(&idgen);
}


#include "catch.hpp"
#include "physics.h"

TEST_CASE( "Kepler Tests", "[kepler]" ) {
    // REQUIRE( 332946==mass_sun/mass_earth );
	 SECTION( "resizing bigger changes size and capacity" ) {
        REQUIRE( G_solar_system==0 );
    }
}
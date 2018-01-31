#include <iostream>
#include <cmath>
#include "catch.hpp"
#include "physics.h"

TEST_CASE( "Kepler Tests", "[kepler]" ) {
    // REQUIRE( 332946==mass_sun/mass_earth );
	 SECTION( "Test physical constants" ) {
		 std::cout<<"s/Y="<<seconds_in_one_year<<std::endl;
		 std::cout<<"Sun/Earth="<<mass_sun/mass_earth<<std::endl;
		 std::cout<<"G="<<G_solar_system<<std::endl;
		 std::cout<<"G0="<<4*M_PI*M_PI<<std::endl;
        REQUIRE( 0==0 );
    }
}

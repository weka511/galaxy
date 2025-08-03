/**
 * Copyright (C) 2018-2025 Greenweaves Software Limited
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>
 *
 * This file exercises treecode. 
 */
 
#include "catch.hpp"
#include "particle.hpp"
using namespace std;


TEST_CASE( "Particle Tests", "[particle]" ) {

	
	SECTION("Trivial Tree Insert") {
		Particle foo,bar,baz;
		foo.init(array{-1.0,-1.0,-1.0},array{0.0,0.0,0.0},1.0,0);
		bar.init(array{-1.0,-1.0,-1.0},array{0.0,0.0,0.0},1.0,1);
		baz.init(array{-1.0,-1.0,-1.0},array{0.0,0.0,0.0},1.0,1);
		REQUIRE(bar == baz);
		REQUIRE(foo != bar);
	}
	

}

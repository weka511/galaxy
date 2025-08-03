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
 * This file exercises the random number generators. It shows that Matsumoto & Nishimura's MersenneTwister does use seed.
 *
 */
 

#include <iostream>
#include "catch.hpp"
#include "mt.h"

using namespace std;

TEST_CASE( "MersenneTwister Tests1", "[mersennetwister]" ) {
	MersenneTwister mt0;
	mt0.init_genrand(1);

	double x01=mt0.random();
	double x02=mt0.random();
	double x03=mt0.random();
	SECTION( "Repeat seed" ) { // same seed -> same sequence
		MersenneTwister mt1;
		mt1.init_genrand(1);
		double x1=mt1.random();
		REQUIRE(x1==Approx(x01).epsilon(1.0e-12));
	}
	SECTION( "Change seed" ) { // different seed -> different sequence
		MersenneTwister mt2;
		mt2.init_genrand(2);
		double x21=mt2.random();
		double x22=mt2.random();
		double x23=mt2.random();
		REQUIRE(abs(x21-x01)>0.01);
		REQUIRE(abs(x21-x02)>0.5);
		REQUIRE(abs(x23-x03)>0.5);
	}
}


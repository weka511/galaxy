/**
 * Copyright (C) 2025 Simon Crase
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
#include "integrators.hpp"
#include <numbers>

using namespace std;
using namespace std::numbers;

class MockAccelerationVisitor : public IAccelerationVisitor{
	/**
	 *  Invoked by Configuration to calculate the acceleration each particle.
	 */
	void visit(Particle & particle) {;}
};


class MockReporter : public IReporter {
  public:
	void visit(Particle & particle) {;}
	
	/**
	 *   Record configuration in a csv file
	 */
	void report() {;}
	
	/**
	 *   Verify that parogram should continue executing,
	 *   i.e. killfile not present
	 */
    bool should_continue() {return true;}
};

TEST_CASE( "Integrator Tests", "[integrator]" ) {
	
	SECTION("1 body test") {
		double params [] = {1.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0};
		Configuration configuration(1, params);
		MockAccelerationVisitor calculate_acceleration;
		MockReporter reporter;
		Leapfrog integrator(configuration,  calculate_acceleration,reporter);
		integrator.run(1,0.1);
	}

}

// REQUIRE(pi == Approx(3.1415926536));
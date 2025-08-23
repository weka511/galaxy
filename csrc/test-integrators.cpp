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
 * This file exercises integrators. 
 */
 
#include <numbers>
#include <cmath>
#include <array>
#include <iostream>
#include <vector>
#include <fstream>
#include "catch.hpp"
#include "integrators.hpp"

using namespace std;
using namespace std::numbers;
using namespace Catch::Matchers;

/**
 *  Perform acceleration calcs for a single particle moving in a central force.
 */
class SingleParticleAccelerationCalculator : public IAccelerationVisitor {
  private:
	Particle _origin;
	
  public:
	SingleParticleAccelerationCalculator() {}
	
	/**
	 *  Invoked by Configuration to calculate the acceleration of each particle.
	 */
	void visit(Particle & particle) {
		auto radius = sqrt(Particle::get_distance_sq(particle,_origin));
		auto denominator = radius * radius * radius;
		auto position = particle.get_position();
		array<double,NDIM> acceleration = {-position[0]/denominator, -position[1]/denominator, -position[2]/denominator};
		particle.set_acceleration(acceleration);
	}
}; 

/**
 * Reporter for tests. Records positions and velocities in a vector and an external file.
 */
class MockReporter : public IReporter {
  private:
	Configuration & _configuration;
	ofstream _output;
	
  public:
  	vector<array<double,NDIM>> positions;
	vector<array<double,NDIM>> velocities;
	
	MockReporter(Configuration  &configuration, string file_name)
	: _configuration(configuration) {
		_output.open(file_name + ".csv");
	}
	
	virtual ~MockReporter(){
		_output.close();
	}
	void visit(Particle & particle) {
		positions.push_back(particle.get_position());
		velocities.push_back(particle.get_velocity());
		_output << particle << endl;
	}
	
	void report() {
		_configuration.iterate(*this);
	}
	
    bool should_continue() {return true;}
	
};

TEST_CASE( "Integrator Tests", "[integrator]" ) {
	
	SECTION("A single particle moving in a circle under a central force") {
		auto n = 10000;
		auto N = 10;
		double params [] = {1.0, 0.0, 0.0, 0.1, 0.0, 1.0, 0.0};
		Configuration configuration(1, params);
		SingleParticleAccelerationCalculator calculate_acceleration;
		MockReporter reporter(configuration,"single-circle");
		Leapfrog integrator(configuration,  calculate_acceleration,reporter);
		configuration.iterate(reporter);
		integrator.run(2*n*N,pi/n);
		REQUIRE_THAT(reporter.positions[0][0], WithinAbs(reporter.positions[2*n*N][0], 1.0e-6));
		REQUIRE_THAT(reporter.positions[0][1], WithinAbs(reporter.positions[2*n*N][1], 1.0e-5));
		REQUIRE_THAT(reporter.velocities[0][0], WithinAbs(reporter.velocities[2*n*N][0], 2.0e-4));
		REQUIRE_THAT(reporter.velocities[0][1], WithinAbs(reporter.velocities[2*n*N][1], 1.0e-6));
	}

}

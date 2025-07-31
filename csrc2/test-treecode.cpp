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
#include "treecode.hpp"

using namespace std;


TEST_CASE( "Tree Tests", "[tree]" ) {
	REQUIRE(Node::get_count() == 0);
	
	SECTION("Trivial Tree Insert") {
		unique_ptr<Particle[]> particles = make_unique<Particle[]>(2);
		auto i = 0;
		particles[i++].init(array{-1.0,-1.0,-1.0},array{0.0,0.0,0.0},0.0,0);
		particles[i++].init(array{-1.0,-1.0,+1.0},array{0.0,0.0,0.0},0.0,0);
		unique_ptr<Node> tree = Node::create(particles,i);
		REQUIRE(Node::get_count() == 9);
	}
	
	SECTION("Larger Tree Insert") {
		unique_ptr<Particle[]> particles = make_unique<Particle[]>(8);
		auto i = 0;
		particles[i++].init(array{-1.0,-1.0,-1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{-1.0,-1.0,+1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{-1.0,+1.0,-1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{+1.0,-1.0,-1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{-1.0,+1.0,+1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{+1.0,-1.0,+1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{+1.0,+1.0,-1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{+1.0,+1.0,+1.0},array{0.0,0.0,0.0},1.0,0);
		unique_ptr<Node> tree = Node::create(particles,i);
		REQUIRE(Node::get_count() == 9);
	}
	
	SECTION("2nd layer Tree Insert") {
		unique_ptr<Particle[]> particles = make_unique<Particle[]>(9);
		auto i = 0;
		particles[i++].init(array{-1.0,-1.0,-1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{-1.0,-1.0,+1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{-1.0,+1.0,-1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{+1.0,-1.0,-1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{-1.0,+1.0,+1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{+1.0,-1.0,+1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{+1.0,+1.0,-1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{+1.0,+1.0,+1.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{+0.2, +0.2, +1.0},array{0.0,0.0,0.0},1.0,0);
		unique_ptr<Node> tree = Node::create(particles,i);
		REQUIRE(Node::get_count() == 17);
	}
	
	SECTION("3rd layer Tree Insert: https://www.cs.princeton.edu/courses/archive/fall03/cs126/assignments/barnes-hut.html") {
		unique_ptr<Particle[]> particles = make_unique<Particle[]>(8);
		auto i = 0;
		particles[i++].init(array{-2.0,2.0,0.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{1.5,3.5,0.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{0.5,2.5,0.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{2.5,0.5,0.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{-3.0,-1.0,0.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{-3.0,-3.0,0.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{-1.0,-3.0,0.0},array{0.0,0.0,0.0},1.0,0);
		particles[i++].init(array{2.0,-2.0,0.0},array{0.0,0.0,0.0},1.0,0);
		unique_ptr<Node> tree = Node::create(particles,i);
		REQUIRE(Node::get_count() == 33);
	}
	REQUIRE(Node::get_count() == 0);
}

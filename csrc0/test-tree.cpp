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
 
#include <vector>
#include "catch.hpp"
#include "treecode.h"
using namespace std;

TEST_CASE( "Tree Tests", "[tree]" ) {
	REQUIRE(Node::_count==0);
	
	
	SECTION("Trivial Tree Insert") {
		vector<Particle*> particles;
		particles.push_back(new Particle(-1,-1,-1,0,0,0,0));
		particles.push_back(new Particle(-1,-1,1,0,0,0,0));
		Node * tree = Node::create(particles);
		REQUIRE(Node::_count==9);
		delete tree;
	}
	
	SECTION("Larger Tree Insert") {
		vector<Particle*> particles;
		particles.push_back(new Particle(-1, -1, -1,0,0,0,1));
		particles.push_back(new Particle(-1, -1, +1,0,0,0,1));
		particles.push_back(new Particle(-1, +1, -1,0,0,0,1));
		particles.push_back(new Particle(+1, -1, -1,0,0,0,1));
		particles.push_back(new Particle(-1, +1, +1,0,0,0,1));
		particles.push_back(new Particle(+1, -1, +1,0,0,0,1));
		particles.push_back(new Particle(+1, +1, -1,0,0,0,1));
		particles.push_back(new Particle(+1, +1, +1,0,0,0,1));
		Node * tree = Node::create(particles);
		REQUIRE(Node::_count==9);
		delete tree;
	}
	
	SECTION("2nd layer Tree Insert") {
		vector<Particle*> particles;
		particles.push_back(new Particle(-1, -1, -1,0,0,0,1));
		particles.push_back(new Particle(-1, -1, +1,0,0,0,1));
		particles.push_back(new Particle(-1, +1, -1,0,0,0,1));
		particles.push_back(new Particle(+1, -1, -1,0,0,0,1));
		particles.push_back(new Particle(-1, +1, +1,0,0,0,1));
		particles.push_back(new Particle(+1, -1, +1,0,0,0,1));
		particles.push_back(new Particle(+1, +1, -1,0,0,0,1));
		particles.push_back(new Particle(+1, +1, +1,0,0,0,1));
		particles.push_back(new Particle(+0.2, +0.2, +1,0,0,0,1));
		Node * tree = Node::create(particles);
		REQUIRE(Node::_count==17);
		delete tree;
	}
	
	REQUIRE(Node::_count==0);
}

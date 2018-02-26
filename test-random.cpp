/**
 * Copyright (C) 2018 Greenweaves Software Limited
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
 * This file exercises the random number generators. It shows that the build-in
 * generators ignore the seed, but that Matsumoto & Nishimura's MersenneTwister does use seed.
 *
 */
 

#include <random>
#include <iostream>
#include "catch.hpp"
#include "mt.h"

TEST_CASE( "Random number generator Tests1", "[random1]" ) {
	std::mt19937_64 generator;
	std::srand(1);
	std::cout << "A time seed produced: " << generator() << std::endl;
	std::cout << "A time seed produced: " << generator() << std::endl;
	std::cout << "A time seed produced: " << generator() << std::endl;
}

TEST_CASE( "Random number generator Tests2", "[random2]" ) {
	std::mt19937_64 generator;
	std::srand(2);
	std::cout << "A time seed produced: " << generator() << std::endl;
	std::cout << "A time seed produced: " << generator() << std::endl;
	std::cout << "A time seed produced: " << generator() << std::endl;
	}
	
TEST_CASE( "Random number generator Tests3", "[random3]" ) {
	std::mt19937_64 generator;
	std::srand(time(NULL));
	std::cout << "A time seed produced: " << generator() << std::endl;
	std::cout << "A time seed produced: " << generator() << std::endl;
	std::cout << "A time seed produced: " << generator() << std::endl;
	}
	
TEST_CASE( "Random number generator Tests4", "[random4]" ) {
	std::mt19937_64 generator;
	std::srand(time(NULL));
	std::cout << "A time seed produced: " << generator() << std::endl;
	std::cout << "A time seed produced: " << generator() << std::endl;
	std::cout << "A time seed produced: " << generator() << std::endl;
	
}

TEST_CASE( "MersenneTwister Tests1", "[mersenne1]" ) {
	MersenneTwister mt;
	mt.init_genrand(1);
    mt.print();
	std::cout << "Seed 1 produced: " << mt.random() << std::endl;
	std::cout << "Seed 1 produced:  " << mt.random() << std::endl;
	std::cout << "Seed 1 produced:  " << mt.random() << std::endl;
}

TEST_CASE( "MersenneTwister Tests2", "[mersenne2]" ) {
	MersenneTwister mt;
	mt.init_genrand(1);
    mt.print();
	std::cout << "Seed 1 produced: " << mt.random() << std::endl;
	std::cout << "Seed 1 produced:  " << mt.random() << std::endl;
	std::cout << "Seed 1 produced:  " << mt.random() << std::endl;
}

TEST_CASE( "MersenneTwister Tests3", "[mersenne3]" ) {
	MersenneTwister mt;
	mt.init_genrand(2);
    mt.print();

	std::cout << "Seed 2 produced: " << mt.random() << std::endl;
	std::cout << "Seed 2 produced:  " << mt.random() << std::endl;
	std::cout << "Seed 2 produced:  " << mt.random() << std::endl;
}

TEST_CASE( "MersenneTwister Tests4", "[mersenne4]" ) {
	MersenneTwister mt;
	mt.init_genrand(time(NULL));
    mt.print();

	std::cout << "Time Seed produced: " << mt.random() << std::endl;
	std::cout << "Time Seed  produced:  " << mt.random() << std::endl;
	std::cout << "Time Seed  produced:  " << mt.random() << std::endl;
}

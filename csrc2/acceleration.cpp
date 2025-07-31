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
 */

#include "acceleration.hpp"
#include "center-of-mass.hpp"
#include "barnes-hut.hpp"

/**
 *  Construct oct-tree from particles, and compute centre of mass for tree and its subtrees
 *
 *    particles
 */
void AccelerationVisitor::create_tree(unique_ptr<Particle[]> &particles, int n) {
	_tree.reset();
	_tree = Node::create(particles,n); 
	CentreOfMassCalculator calculator(particles,n);
	_tree->traverse(calculator);
	calculator.check_all_particles_processed();
}

/**
 *  Calculate acceleration for one node only
 */
void AccelerationVisitor::visit(int i,Particle & particle){
	cout << __FILE__ << " " << __LINE__ << endl;
	BarnesHutVisitor visitor(i,particle,_theta,_G,_softening_length);
	_tree->traverse(visitor);
	visitor.store_accelerations();
}


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
#include "logger.hpp"

/**
 *  Create acceleration visitor
 *
 *  Parameters:
 *      configuration	Container for particles
 *		theta           Ratio for Barnes G=Hut cutoff (Barnes and Hut recommend 1.0)
 *      G				Gravitational constant
 *      a				Softening length
 */
AccelerationVisitor::AccelerationVisitor(const double theta,const double G,const double a, bool verify_tree)
	 : _theta(theta),_G(G),_a(a), _verify_tree(verify_tree){}
	 
/**
 *  Construct oct-tree from particles, and compute centre of mass for tree and its subtrees
 *
 *  Parameters:
 *      particles    Pointer to particles
 *      n            Number of particles
 */
void AccelerationVisitor::initialize(unique_ptr<Particle[]> & particles, int n)  {
	_tree.reset();
	_tree = Node::create(particles,n,_verify_tree); 
	CentreOfMassCalculator calculator(particles);
	_tree->traverse(calculator);
}

/**
 *  Calculate acceleration for one node only. The real work is delegated to the Barnes Hut Visitor,
 *  which computes the force on this particles from each other particle.
 *
 *  Parameters:
 *      particle   The particle whose acceleration is to be computed
 */
void AccelerationVisitor::visit(Particle & particle){
	BarnesHutVisitor visitor(particle,_theta,_G,_a);
	_tree->traverse(visitor);
	visitor.store_accelerations();
}


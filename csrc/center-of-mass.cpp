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
 
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>
#include <cassert>

#include "center-of-mass.hpp"

using namespace std;

 /**
  * Create CentreOfMassCalculator and mark all particles not processed
  *
  * Parameters:
  *   	particles These are the particles whose centre of mass is to be calculated. 
  */
CentreOfMassCalculator::CentreOfMassCalculator(unique_ptr<Particle[]> &particles, int n) 
  : _particles(particles) {
	for (int i=0;i<n;i++)
		_processed_particle.push_back(false);
}

/**
 * Called for each node, but unly does something for an External Node:
 * record the position and mass of the particle
 */
Node::Visitor::Status CentreOfMassCalculator::visit(Node * node) {
	const int particle_index = node->getType();
	if (particle_index >= 0)
		record_particle(node,particle_index);
	return Node::Visitor::Status::Continue;
}

/**
 * Record the position and mass of a particle
 *
 * Parameters:
 *     node             An external node (this is visit()'s responsibility)
 *     particle_index   Identifies the particle being recorded
 */
void CentreOfMassCalculator::record_particle(Node * node,const int particle_index) {   
	Particle & particle = _particles[particle_index];
	node->set_mass_and_centre(particle.get_mass(),particle.get_position());
	_processed_particle[particle_index] = true;
}

/**
 *  We need to accumulate the mass and positions for each child of an internal node
 */
void CentreOfMassCalculator::accumulate(Node * node,Node * child){
	assert (node->getType() == Node::Internal); 
	node->accumulate_center_of_mass(child);
}

/**
 * Make sure every node was processed
 */
void CentreOfMassCalculator::verify_all_particles_processed() {
	for (std::vector<bool>::size_type i =0;i<_processed_particle.size();i++)
		if (!_processed_particle[i]) {
			stringstream message;
			message<<__FILE__ <<", " <<__LINE__<<" Missing index: "<<_processed_particle[i]<<endl; 
			throw logic_error(message.str().c_str());
		}
}

/**
 * This is called when we finish processing a Node, which means that all children 
 * have been processed. At this statge we have accumulated total mass, and a weighted 
 * sum of positions of centres for children. Divide total by mass,
 * and store centre of mass.
 */
void CentreOfMassCalculator::depart(Node * node)  {
	assert(node->getType() == Node::Internal);
	array<double,3> X;
	double m;
	tie(m,X) = node->get_mass_and_centre();
	for (int i=0;i<3;i++)
		X[i] /= m;
	node->set_mass_and_centre(m,X);

	assert( node->verify_within_bounding_box());
}


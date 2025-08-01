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

#include "center-of-mass.hpp"

using namespace std;

 /**
  * Create CentreOfMassCalculator and mark all particles not processed
  *  Parameters:
  *   	particles These are the particles whose centre of mass is to be calculated. 
  */
CentreOfMassCalculator::CentreOfMassCalculator(unique_ptr<Particle[]> &particles, int n) 
  : _particles(particles) {
	for (int i=0;i<n;i++)
		_processed_particle.push_back(false);
}

/**
 * When we visit an External Node, record the position and mass of the particle
 */
Node::Visitor::Status CentreOfMassCalculator::visit(Node * node) {
	const int particle_index= node->getStatus();
	if (particle_index >= 0)
		record_particle(node,particle_index);
	return Node::Visitor::Status::Continue;
}

/**
 * Record the position and mass of the particle
 */
void CentreOfMassCalculator::record_particle(Node * node,const int particle_index) {   
	_processed_particle[particle_index] = true;
	auto pos = _particles[particle_index].get_position();
	double x,y,z;
	x = pos[0];
	y = pos[1];
	z = pos[2];
	node->set_mass_and_centre(_particles[particle_index].get_mass(),x,y,z);
}

/**
 *  For an internal note we need to accumulate the mass and positions for each child
 */
void CentreOfMassCalculator::farewell(Node * node,Node * child){
	if (node->getStatus() == Node::Internal) 
		node->accumulatePhysics(child);
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
 *This is called when we finish processing a Node, which means that all children 
 * have been processed. Store centre of mass.
 */
bool CentreOfMassCalculator::depart(Node * node)  {
	double m,x,y,z;
	tie(m,x,y,z) = node->get_mass_and_centre();
	cout <<__FILE__ <<", " <<__LINE__<< ": " << node->getStatus()
		<<" (" <<x <<"," << y<< "," <<z<< ") " << m << endl;
	switch (node->getStatus()) {
		case Node::Internal:
			node->set_mass_and_centre(m,x/m,y/m,z/m);
			break;
		case Node::Unused:
			return true;
		default: ;
	}
	return node->verify_within_bounding_box(x,y,z);
}


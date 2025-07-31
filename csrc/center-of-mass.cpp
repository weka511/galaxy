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
 */
 
#include "center-of-mass.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>
#include <stdlib.h>
using namespace std;
CentreOfMassCalculator::CentreOfMassCalculator(vector<Particle*> particles) 
  : _particles(particles) {
	for (int i=0;i<particles.size();i++)
		_processed_particle.push_back(false);
}

/**
 * When we visit an External Node, record the position and mass of the particle
 */
Node::Visitor::Status CentreOfMassCalculator::visit(Node * node) {
	const int particle_index= node->getStatus();
	if (particle_index>=0) {
		_processed_particle[particle_index]=true;
		double x,y,z;
		_particles[particle_index]->getPos(x,y,z);
		node->setPhysics(_particles[particle_index]->getMass(),x,y,z);
	}
	return Node::Visitor::Status::Continue;
}

/**
 *  For an internal note we need to accumulate the mass and positions for each child
 */
void CentreOfMassCalculator::propagate(Node * node,Node * child){
	if (node->getStatus()==Node::Internal) 
		node->accumulatePhysics(child);
}

/**
 * Make sure every node was processed
 */
void CentreOfMassCalculator::check_all_particles_processed() {
	for (int i =0;i<_processed_particle.size();i++)
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
	node->getPhysics(m,x,y,z);
	switch (node->getStatus()) {
		case Node::Internal:
			x/=m;y/=m;z/=m;
			node->setPhysics(m,x,y,z);
			break;
		case Node::Unused:
			return true;
		default: ;
	}
	if (x<node->_xmin || node->_xmax<x || y<node->_ymin || node->_ymax<y || z<node->_zmin || node->_zmax<z) {
		cerr<<__FILE__ <<", " <<__LINE__<< "Status: "<< node->getStatus()<<endl;   // FIXME - throw exception
		cerr << node->getStatus()<< " "<<node->_xmin << ", " << x << ", " << node->_xmax << endl;
		cerr << node->getStatus()<< " "<<node->_ymin << ", " << y << ", " << node->_ymax << endl;
		cerr << node->getStatus()<< " "<<node->_zmin << ", " << z << ", " << node->_zmax << endl;
		stringstream message;
		message<<__FILE__ <<", " <<__LINE__<<" Centre of mass out of range - see logfile."<<endl; 
		throw logic_error(message.str().c_str()); 
	}
	return true;
}


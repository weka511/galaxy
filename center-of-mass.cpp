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
 */
 
#include "center-of-mass.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <stdlib.h>

CentreOfMassCalculator::CentreOfMassCalculator(std::vector<Particle*> particles) 
: _particles(particles) {
	for (int i=0;i<particles.size();i++)
		indices.push_back(false);
}

Node::Visitor::Status CentreOfMassCalculator::visit(Node * node) {
	const int index= node->getStatus();
	if (index>=0) {
		indices[index]=true;
		double x,y,z;
		_particles[index]->getPos(x,y,z);
		node->setPhysics(_particles[index]->getMass(),x,y,z);
	}
	return Node::Visitor::Status::Continue;
}

void CentreOfMassCalculator::propagate(Node * node,Node * child){
	const int index= node->getStatus();
	if (index==Node::Internal) {
		node->accumulatePhysics(child);
	}
}

void CentreOfMassCalculator::display() {
	std::cout << "Done " << indices.size() << " bodies."<< std::endl;
	for (int i =0;i<indices.size();i++)
		if (!indices[i])
			std::cout<<"Missing index: "<<indices[i]<<std::endl;
}

bool CentreOfMassCalculator::depart(Node * node)  {
	double m,x,y,z;
	node->getPhysics(m,x,y,z);
	switch (node->getStatus()) {
		case Node::Internal:
			x/=m;y/=m;z/=m;
			break;
		case Node::Unused:
			return true;
		default: ;
	}
	if (x<node->_xmin || node->_xmax<x || y<node->_ymin || node->_ymax<y || z<node->_zmin || node->_zmax<z) {
		std::cout << node->getStatus()<< " "<<node->_xmin << ", " << x << ", " << node->_xmax << std::endl;
		std::cout << node->getStatus()<< " "<<node->_ymin << ", " << y << ", " << node->_ymax << std::endl;
		std::cout << node->getStatus()<< " "<<node->_zmin << ", " << z << ", " << node->_zmax << std::endl;
	}
	return true;
}


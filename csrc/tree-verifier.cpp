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
 
// #include <limits>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <sstream>

#include "treecode.hpp"
#include "tree-verifier.hpp"

using namespace std;

/**
 *  Build the data structure needed to verify that each  
 *  particle is in the Tree once an only once.
 */
TreeVerifier::TreeVerifier(unique_ptr<Particle[]> &particles, const int n) : _particles(particles),_n(n) {
	_particle_verified = vector<bool>();
	for (int i=0;i<n;i++)
		_particle_verified.push_back(false);
}

/**
 *  This function initializes the data structure that is used to verify
 *  that subtrees are nested properly. The data structure is filled in
 *  by accumulate() and checked by depart() 
 */ 
Node::Visitor::Status TreeVerifier::visit_internal(Node * node){
	array<array<array<bool,2>,2>,2> block;
	for (int i=0;i<2;i++)
		for (int j=0;j<2;j++)
			for (int k=0;k<2;k++)
				block[i][j][k] = false;
	_child_within_limits.push_back(block);
	return Node::Visitor::Status::Continue;
}

/**
 *  Verify that each particle is in the Tree once and only once.
 */
Node::Visitor::Status TreeVerifier::visit_external(Node * node){
	const int index = node->get_index();
	assert(!_particle_verified[index]);
	_particle_verified[index] = true;
	return Node::Visitor::Status::Continue;
}

/**
 *  This function is used to verify that subtrees are nested properly. It tests one axis to see
 *  whether it belongs to upper or lower half
 */
int TreeVerifier::_get_index(Node * node,Node * child,const int i,double tolerance) {
	if (abs(node->_Xmin[i] - child->_Xmin[i]) && abs(node->_Xmean[i] - child->_Xmax[i])) return 0;
	if (abs(node->_Xmax[i] - child->_Xmax[i]) && abs(node->_Xmean[i] - child->_Xmin[i])) return 1;
	stringstream message;
	message<<__FILE__ <<" " <<__LINE__<<" Error: determine index " <<i << endl; 
	throw logic_error(message.str().c_str()); 
}

/**
 *  This function is used to verify that subtrees are nested properly. 
 *  It tests a child node to see which octant it belongs to. Later the 8
 *  and checked by depart() 
 */ 
void TreeVerifier::accumulate(Node * node,Node * child){
	auto i = _get_index(node,child,0);
	auto j = _get_index(node,child,1);
	auto k = _get_index(node,child,2);
	assert(!_child_within_limits.back()[i][j][k]);
	_child_within_limits.back()[i][j][k]	= true;
}

/**
 *  This function is used to verify that subtrees are nested properly. 
 *  It checks the collection of octants build by visit_internal()
 *  and accumulate() to ensure that it is complete.
 */ 
void TreeVerifier::depart(Node * node) {
	for (int i=0;i<2;i++)
		for (int j=0;j<2;j++)
			for (int k=0;k<2;k++)
				assert(_child_within_limits.back()[i][j][k]);
	_child_within_limits.pop_back();
}

/**
 *  Used to verify that all particles have been added
 */
bool TreeVerifier::has_been_verified(){
	for (bool status : _particle_verified)
		if (!status)
			return false;
	assert(_child_within_limits.size()==0);
	return true;
}
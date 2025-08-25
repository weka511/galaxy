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
 
#include <limits>
#include <cassert>

#include "treecode.hpp"

using namespace std;

int Node::_count=0;   // Initially tree is empty

/**
 * Create an oct-tree from a set of particles
 */
unique_ptr<Node> Node::create(unique_ptr<Particle[]> &particles, int n,const bool verify,const double pad){
	double zmin, zmax;
	tie(zmin,zmax) = _get_limits(particles,n,pad);
	array<double,NDIM> Xmin = {zmin,zmin,zmin};
	array<double,NDIM> Xmax = {zmax,zmax,zmax};
	unique_ptr<Node> product = unique_ptr<Node>(new Node(Xmin,Xmax));

	for (int index=0;index<n;index++)
		product->insert(index,particles);

	if (verify){
		TreeVerifier verifier(particles,n);
		product->traverse(verifier);
		assert(verifier.has_been_verified());
	}
	return product;
}

/**
 * Determine a cube that will serve as a bounding box for the
 * set of particles.  Make it slightly larger than strictly
 * needed, so everything is guaranteed to be inside box
 *
 * Parameters:
 *     particles    The paritcles
 *     n			Number of particles
 *     pad			Box will be expanded by a factor of (1+pad)
 */
tuple<double,double> Node::_get_limits(unique_ptr<Particle[]>& particles,int n,const double pad){
	auto zmin = numeric_limits<double>::max();
	auto zmax = -zmin;
	for (int i=0;i<n;i++)
		for (int j=0;j<NDIM;j++){
			zmin = min(zmin,particles[i].get_position()[j]);
			zmax = max(particles[i].get_position()[j],zmax);
		}

	if (zmin < 0)
		zmin *= (1+pad);
	else 
		zmin /= (1+pad);
	
	if (zmax < 0)
		zmax /= (1+pad);
	else
		zmax *= (1+pad);
	
	return make_tuple(zmin,zmax);
}


/**
 * Create a Node for a given region of space. Set it Unused until 
 * it has been added to Tree and has at least one child node
 */
 
Node::Node(array<double,NDIM> Xmin,array<double,NDIM> Xmax)
  : _id(_count),_particle_index(Unused),
  	_m(0.0), _center_of_mass({0.0,0.0,0.0}),
	_Xmin(Xmin), _Xmax(Xmax){
	for (int i=0;i<NDIM;i++)
		_Xmean[i] = 0.5 * (Xmin[i] + Xmax[i]);
	for (int i=0;i<N_Children;i++)
		_child[i] = NULL;
	_count++;
}


/**
 * Insert one particle in tree.
 *
 * Recursively descend until we find an empty node.
 */
void Node::insert(int new_particle_index,unique_ptr<Particle[]> &particles) {

	switch(_particle_index){
		case Unused:                              // This Node is currently Unused
			_particle_index = new_particle_index; // so we can add particle to it, making it External
			return;
		case Internal: { 																// This Node is Internal
			Node * subtree = _child[_get_octant_number(particles[new_particle_index])]; // so we can add particle to the appropriate subtree
			subtree->insert(new_particle_index,particles);
			return;
		}
		default:                                                                   // This Node is External
			_split_and_insert_below(new_particle_index,_particle_index,particles); // so we already have a particle here; we have to move it
	}
}

/**
 * Find correct subtree to store particle, using bounding rectangular box.
 * We split x, y, and z into halves, and determine which half each axis belongs to
 */
int Node::_get_octant_number(Particle &particle) {
	auto pos = particle.get_position();
	array<int,NDIM> indices;
	for (int i=0;i<NDIM;i++)
		indices[i] = (pos[i] > _Xmean[i]);
	
	return _triple_to_octant(indices);
}


/**
 * Split an External node, and insert the new particle and the incumbent into subtrees.
 */
void Node::_split_and_insert_below(int new_particle_index,int incumbent,unique_ptr<Particle[]> &particles) {
	_split_node();
	_insert_or_propagate(new_particle_index,incumbent,particles);
} 

/**
 * Used when we have just split an External node, so we need to pass
 * the incumbent and a new particle down the tree
 */
void Node::_insert_or_propagate(int new_particle_index,int incumbent,unique_ptr<Particle[]> &particles) {
	const int child_index_new = _get_octant_number(particles[new_particle_index]);
	const int child_index_incumbent = _get_octant_number(particles[incumbent]);
	if (child_index_new ==  child_index_incumbent)
		_child[child_index_incumbent]->_split_and_insert_below(new_particle_index,incumbent,particles);
	else {
		_child[child_index_new]->insert(new_particle_index,particles);
		_child[child_index_incumbent]->insert(incumbent,particles);
	}
}

/**
 * Convert an External Node into an Internal one, and
 * partition bounding box into eight, one for each child, so we can 
 * assign particle to a member of the partition. We end up with 8
 * unused nodes below this one.
 */
void Node::_split_node() {
	_particle_index = Internal;
	array<double,NDIM> Xmin;
	array<double,NDIM> Xmax;
	for (int i=0;i<2;i++) {
		tie (Xmin[0], Xmax[0]) = _get_refined_bounds(i, _Xmin[0], _Xmax[0],  _Xmean[0]);
		for (int j=0;j<2;j++) {
			tie (Xmin[1], Xmax[1])  = _get_refined_bounds(j, _Xmin[1], _Xmax[1],  _Xmean[1]);
			for (int k=0;k<2;k++) {
				tie (Xmin[2], Xmax[2])  = _get_refined_bounds(k, _Xmin[2],  _Xmax[2],  _Xmean[2]);
				_child[_triple_to_octant(i,j,k)] = new Node(Xmin, Xmax);
			}
		}
	}
} 

/**
 * Traverse Tree, visiting each node depth first.  The Visitor decides whether
 * we continue all the way down. After visiting each child of the current node
 * we call accumulate() on the node itself, and then finally call depart() on 
 * the current node after all children processed.
 */
void Node::traverse(Visitor & visitor) {
	switch (_particle_index) {
		case Internal:
			visitor.visit_internal(this);
			for (int i=0;i<N_Children;i++) {
				_child[i]->traverse(visitor);
				visitor.accumulate(this,_child[i]);
			}
			visitor.depart(this);
			return;
		case Unused:
			return;
		default:
			visitor.visit_external(this);
	}
}

/**
 *   Used to calculate centre of mass for internal nodes.
 *   We accumulate mx,my,mz, so we need to treat External nodes differently
 */
void Node::accumulate_center_of_mass(Node* child) {
	_m += child->_m;
	for (int i=0;i<NDIM;i++)
		_center_of_mass[i] += child->_m * child->_center_of_mass[i];
}

/**
 * Used to delete tree
 */ 
Node::~Node() {
	for (int i=0;i<N_Children;i++)
		if (_child[i] != NULL)
			delete _child[i];

	Node::_count--;
}

/**
 * Used to get number of Nodes - normally to make sure tree has been destructed.
 */
int Node::get_count() {return _count;}

TreeVerifier::TreeVerifier(unique_ptr<Particle[]> &particles, const int n) : _particles(particles),_n(n) {
	_particle_verified = vector<bool>();
	for (int i=0;i<n;i++)
		_particle_verified.push_back(false);
}
	
Node::Visitor::Status TreeVerifier::visit_internal(Node * node){
	return Node::Visitor::Status::Continue;
}

Node::Visitor::Status TreeVerifier::visit_external(Node * node){
	const int index = node->get_index();
	assert(!_particle_verified[index]);
	_particle_verified[index] = true;
	return Node::Visitor::Status::Continue;
}

void TreeVerifier::accumulate(Node * node,Node * child){
}

bool TreeVerifier::has_been_verified(){
	for (bool status : _particle_verified)
		if (!status)	return false;
	return true;
}
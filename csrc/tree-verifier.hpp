#ifndef _TREE_VERIFIER_HPP
#define _TREE_VERIFIER_HPP

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
 
#include <vector>
 
#include "treecode.hpp"

using namespace std;

 /**
 * This class verfies that Tree has been built correctly
 */
class TreeVerifier: public Node::Visitor{
  private:
    unique_ptr<Particle[]> &_particles;

	const int _n;
 
	vector<bool> _particle_verified;
	
	vector<array<array<array<bool,2>,2>,2>> _child_within_limits;
	
   public:
	
	TreeVerifier(unique_ptr<Particle[]> &particles, const int n);
	
	/**
	 *  This function initializes the data structure that is used to verify
	 *  that subtrees are nested properly. The data structure is filled in
	 *  by accumulate() and checked by depart() 
	*/ 
	Node::Visitor::Status visit_internal(Node * node);

	/**
	 *  Used to verify that all particles have been added
	 */
	Node::Visitor::Status visit_external(Node * node);
	
	/**
	 *  This function is used to verify that subtrees are nested properly. 
	 *  It tests a child node to see which octant it belongs to. Later the 8
	 *  and checked by depart() 
	 */ 
	void accumulate(Node * node,Node * child);
	
	/**
	 *  This function is used to verify that subtrees are nested properly. 
	 *  It checks the collection of octants build by visit_internal()
	 *  and accumulate() to ensure that it is complete.
	 */ 
	void depart(Node * node);
	
	/**
	 *  Used to verify that all particles have been added
	 */
	bool has_been_verified();
	
  private:
	/**
	 *  This function is used to verify that subtrees are nested properly. It tests one axis to see
	 *  whether it belongs to upper or lower half
	 */
    int _get_index(Node * node,Node * child,const int i, double tolerance=0.000001);
			
};
 
 #endif // _TREE_VERIFIER_HPP
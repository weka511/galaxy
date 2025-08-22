#ifndef _CENTRE_OF_MASS_HPP
#define _CENTRE_OF_MASS_HPP

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
 
#include "treecode.hpp"

using namespace std;

/**
 * This class is used to calculate centre of mass for each Node in the Oct Tree.
 */
class CentreOfMassCalculator : public Node::Visitor {
	
  private:
   /**
    * These are the particles whose centre of mass is to be calculated. 
    */
	unique_ptr<Particle[]> & _particles;
	
  public:
  
    /**
    * Create CentreOfMassCalculator.
	*   Parameters:
	*   	particles These are the particles whose centre of mass is to be calculated. 
    */
	CentreOfMassCalculator(unique_ptr<Particle[]> &particles);
	
	/**
	 * Called for each internal node: dummy -- does nothing.
	 */
	Node::Visitor::Status visit_internal(Node * internal_node) {
		return Node::Visitor::Status::Continue;
	}
	
	/**
	 * Called for each external node: record the position and mass of the particle
	 */
	Node::Visitor::Status visit_external(Node * external_node);
	
	
	/**
	 *  For an internal note we need to accumulate the mass and positions for each child
	 */
	virtual void accumulate(Node * node,Node * child);
	
	/**
	* This is called when we finish processing a Node, which means that all children 
	* have been processed. Store centre of mass.
	*/
	virtual void depart(Node * node);
	
};

#endif   //_CENTRE_OF_MASS_HPP

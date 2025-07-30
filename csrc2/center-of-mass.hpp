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
 
#ifndef _CENTRE_OF_MASS_HPP
#define _CENTRE_OF_MASS_HPP

#include <vector>
#include "treecode.hpp"

using namespace std;

/**
 * Used to calculate centre of mass for Internal and External Nodes
 */
class CentreOfMassCalculator : public Node::Visitor {
  public:
    /**
    * Create CentreOfMassCalculator.
	*   Parameters:
	*   	particles These are the particles whose centre of mass is to be calculated. 
    */
	CentreOfMassCalculator(unique_ptr<Particle[]> &particles, int n);
	/**
	 * When we visit an External Node, record the position and mass of the particle
	 */
	Node::Visitor::Status visit(Node * node);
	
	/**
	 *  For an internal note we need to accumulate the mass and positions for each child
	 */
	virtual void propagate(Node * node,Node * child);
	
	/**
	* This is called when we finish processing a Node, which means that all children 
	* have been processed. Store centre of mass.
	*/
	virtual bool depart(Node * node);
	
	/**
	 * Make sure every node was processed.
	*/
	void check_all_particles_processed();
	
  private:
  
   /**
    * These are the particles whose centre of mass is to be calculated. 
    */
	unique_ptr<Particle[]> & _particles;
	
	/**
	 * Indicates whether each particle has been processed
	 */
	vector<bool> _processed_particle; 
};

#endif   //_CENTRE_OF_MASS_HPP

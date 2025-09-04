#ifndef _ACCELERATION_HPP
#define _ACCELERATION_HPP

/**
 * Copyright (C) 2025 Greenweaves Software Limited
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
 
#include "configuration.hpp"
#include "treecode.hpp"

using namespace std;

class IAccelerationVisitor : public Visitor<Particle>, public Configuration::ParticleInitializer{
	virtual void initialize(unique_ptr<Particle[]> & particles, int n) {;}
};

/**
 * This class calculates the acceleration for each particle.
 */
class AccelerationVisitor : public IAccelerationVisitor {
	
  private:
	/**
	 * Oct Tree containing particles
	 */
	unique_ptr<Node> _tree = NULL;
	
	/**
	 *   Ratio for Barnes G=Hut cutoff (Barnes and Hut recommend 1.0)
	 */
	const double _theta;
	
	/**
	 *   Gravitational constant
	 */
	const double _G;
	
	/**
	 *   Softening length
	 */
	const double _a; 
	
	bool _verify_tree;
	
  public:
	/**
	 *  Create acceleration visitor
	 *
	 *  Parameters:
	 *      configuration	Container for particles
	 *		theta           Ratio for Barnes G=Hut cutoff (Barnes and Hut recommend 1.0)
	 *      G				Gravitational constant
	 *      a				Softening length
	 */
    AccelerationVisitor(Configuration& configuration, const double theta,const double G,const double a, const bool verify_tree);
	 
	/**
	 *  Construct oct-tree from particles
	 *
	 *  Parameters:
	 *      particles    Pointer to particles
	 *      n            Number of particles
	 */
	void initialize(unique_ptr<Particle[]> & particles, int n);
	
	/**
	 *  Invoked by Configuration to calculate the acceleration of each particle.
	 *
	 *  Parameters:
	 *      particle   The particle whose acceleration is to be computed
	 */
	void visit(Particle & particle);

};

#endif  // _ACCELERATION_HPP

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

class AccelerationVisitor : public Configuration::Visitor, public Configuration::ParticleInitializer{
  public:
    AccelerationVisitor(Configuration& configuration, const double theta,const double G,const double softening_length)
	 : _theta(theta),_G(G),_softening_length(softening_length){;}
	 
	/**
	 *  Construct oct-tree from particles
	 *
	 *    particles
	 */
	void initialize(int n, unique_ptr<Particle[]> & particles);
	
	void visit(int i,Particle & particle);

  private:
	unique_ptr<Node> _tree = NULL;
	const double _theta;
	const double _G;
	const double _softening_length;
};

#endif  // _ACCELERATION_HPP

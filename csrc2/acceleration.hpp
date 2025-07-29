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
 
#ifndef _ACCELERATION_HPP
#define _ACCELERATION_HPP

#include <vector>
#include "configuration.hpp"
#include "treecode.hpp"

 using namespace std;

class AccelerationVisitor : public Configuration::CompoundVisitor{
  public:
    AccelerationVisitor(Configuration& configuration, const double theta,const double G,const double softening_length){;}
	/**
	 *  Construct oct-tree from particles
	 *
	 *    particles
	 */
	void create_tree(unique_ptr<Particle[]> &particles, int n);
	void visit(Particle & particle);
	void visit_pair(Particle & particle1,Particle & particle2);

  private:
   unique_ptr<Node> _tree = NULL;
};

#endif

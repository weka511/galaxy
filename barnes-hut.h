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
 

#ifndef _BARNES_HUT_H
#define _BARNES_HUT_H
#include <vector>
#include "particle.h"
#include "treecode.h"
#include "utils.h"

/**
 * Calculate acceleration for all particles
 */
void get_acceleration(std::vector<Particle*>&,const double theta,const double G,const double softening_length);

/**
 *  Construct oct-tree from particles
 *
 *    particles
 */
Node * create_tree(std::vector<Particle*>& particles);

/**
 * Calculate acceleration for one specific particle
 */
void get_acceleration(int i, std::vector<Particle*>& particles,Node * root,const double theta,const double G,const double a);

/**
 *  This class is used to calculate the acceleration of one particle 
 *  using the Barnes Hut algorithm
 *
 */
class BarnesHutVisitor :  public Node::Visitor{
  public:
   /**
   * Initialize BarnesHutVisitor for a specific particle
   *
   *  index   Keep track of particle index so we don't calculate acceleration of particle caused by itself!
   *  me      Particle being processed
   *  theta   Angle for Barnes G=Hut cutoff
   *  G       Gravitational constant
   *  a       Softening length
   */
	BarnesHutVisitor(const int index,Particle* me,const double theta, const double G,const double a);
	
	/**
	 * Used to accumulate accelerations for each node
	 */
	virtual Node::Visitor::Status visit(Node * node);
	
	/**
	 * Used at the end of calculation to store accelerations back into particle
	 */
	void store_accelerations();
	
  private:
  
	/**
	 * Keep track of particle index so we don't calculate acceleration of particle caused by itself!
	 */
	const int _index;
	/**
	 * Used to add in the contribution to the acceleration from one Node
	 */
	void _accumulate_acceleration(double m,double x,double y,double z,double dsq);
	
	/**
	 * The particle whose acceleration is being calculated
	 */
	Particle * _me;
	
	/**
	 * Store squared theta to simplify comparisons
	 */ 
	const double _theta_squared;

	/**
	 * Position of the particle whose acceleration is being calculated
	 */
	double _x;
	double _y;
	double _z;
	
	/**
	 * Gravitational constant
	 */
	const double _G;
	
	/**
	 * Softening length
	 */
	const double _a;
	
	/**
	 * We accumulate the acceleration here
	 */
	double _acc_x;
	double _acc_y;
	double _acc_z;
};

#endif
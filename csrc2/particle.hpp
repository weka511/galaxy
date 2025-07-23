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
 
#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

#include <array>
#include <iostream>
#include <ostream>
#include "utils.hpp"
using namespace std;

/**
 * A Particle represents one of the bodies whose motion is being simulated.
 * It is just a passive container for data
 */
class Particle {
  private:
    array<double,3> _position;
	array<double,3> _velocity;
	array<double,3> _acceleration;
	double _m;
	
  public:
	void init(array<double,3> position, array<double,3> velocity, double m);
	
	/**
	 *  Accessor for mass
	 */
	inline double get_mass() {return _m;}
	
	inline void getPos(double& x,	double& y, double& z) {x=_position[0];y=_position[1];z=_position[2];}

	inline double get_distance_sq(Particle&other)  {
		return sqr(_position[0] - other._position[0]) + sqr(_position[1] - other._position[1]) + sqr(_position[2] - other._position[2]);
	}
	/**
     * Output position, velocity, and mass.
     */
	friend ostream& operator<<(ostream& s, Particle& p);
	
};

#endif //_PARTICLE_HPP

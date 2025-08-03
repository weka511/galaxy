#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

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
 


#include <array>
#include <iostream>
#include <ostream>

using namespace std;

/**
 * Square a number
 */ 
inline double sqr(double x) {return x*x;}

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
	int _id;
	
  public:
	void init(array<double,3> position, array<double,3> velocity, double m, int id);
	
	/**
	 *  Accessor for mass
	 */
	inline double get_mass() {return _m;}
	
	inline int get_id() {return _id;}
	
	inline array<double,3> & get_position() {return _position;} 
	
	inline void set_position(array<double,3> &  position) {_position = position;}
	
	inline array<double,3> & get_velocity() {return _velocity;}  
	
	inline void set_velocity(array<double,3> &  velocity) {_velocity = velocity;}
	
	inline array<double,3> & get_acceleration() {return _acceleration;}  
	
	inline void set_acceleration(array<double,3> &  acceleration) {_acceleration = acceleration;}
	
	inline double get_distance_sq(Particle&other)  {
		return sqr(_position[0] - other._position[0]) + sqr(_position[1] - other._position[1]) + sqr(_position[2] - other._position[2]);
	}
	
	void reset_acceleration() {};   // TODO
	
	void accumulate_acceleration (double x1,double y1,double z1) {};   // TODO
	
	/**
     * Output position, velocity, and mass.
     */
	friend ostream& operator<<(ostream& s, Particle& p);
	
	/**
	 * The == operator is used when we calculate the attraction between particles
	 * to ensure that a particle doesn't attract itself.
	 */
	bool operator == (const Particle & other)  const {return _id == other._id;} 
	
};



#endif //_PARTICLE_HPP

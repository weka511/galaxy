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
 *   Number of dimensions in space
 */
const int NDIM = 3;

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
  	/**
	 *  The ID of a particle is equal to its index in the array of configurations
	 */
	int _id;
	
	/**
	 *  Location of particle
	 */
    array<double,NDIM> _position;
	
	/**
	 *   Velocity of particle
	 */
	array<double,NDIM> _velocity;
	
	/**
	 *  Acceleration of particle
	 */
	array<double,NDIM> _acceleration;
	
	/**
	 * Mass of particle
	 */
	double _m;
	
  public:
  
	/**
	 *   Create one stationary particle at origin with unit mass
	 */
	Particle();
	
  	/**
	 * Determine squared distance between two particles
	 */
	static inline double get_distance_sq(Particle&particle1,Particle&particle2)  {
		double sum = 0.0;
		for (int i=0;i<NDIM;i++)
			sum += sqr(particle1._position[i] - particle2._position[i]);
		return sum;
	}
	
	static inline double get_distance_sq(array<double,NDIM> position1,array<double,NDIM> position2)  {
		double sum = 0.0;
		for (int i=0;i<NDIM;i++)
			sum += sqr(position1[i] - position2[i]);
		return sum;
	}
	
	/**
	 *   Used to set initial position and velocity when configuration is initialized.
	 */
	void init(array<double,NDIM> position, array<double,NDIM> velocity, double m, int id);
	
	/**
	 *  Accessor for mass
	 */
	inline double get_mass() {return _m;}
	
	/**
	 *  Accessor for ID
	 */
	inline int get_id() {return _id;}
	
	/**
	 *  Accessor for position
	 */
	inline array<double,NDIM> & get_position() {return _position;} 
	
	/**
	 *  Used to assign a new position
	 */
	inline void set_position(array<double,NDIM> &  position) {_position = position;}
	
	/**
	 *  Accessor for velocity
	 */
	inline array<double,NDIM> & get_velocity() {return _velocity;}  
	
	/**
	 *  Used to assign a new velocity
	 */
	inline void set_velocity(array<double,NDIM> &  velocity) {_velocity = velocity;}
	
	/**
	 *  Accessor for acceleration
	 */
	inline array<double,NDIM> & get_acceleration() {return _acceleration;}  
	
	/**
	 *  Used to assign acceleration
	 */
	inline void set_acceleration(array<double,NDIM> &  acceleration) {_acceleration = acceleration;}
	 

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

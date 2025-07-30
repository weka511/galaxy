#ifndef _PHYSICS_HPP
#define _PHYSICS_HPP

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
 


#include <memory>
#include "configuration.hpp"

using namespace std;

class Physics {
  public:
	static const double seconds_in_one_year;

	static const double mass_earth;

	static const double mass_sun ;  // https://en.wikipedia.org/wiki/Solar_mass

	static const double metres_per_AU ; // https://en.wikipedia.org/wiki/Astronomical_unit

	static const double au_per_m;

	static const double G_SI ; // https://en.wikipedia.org/wiki/Gravitational_constant

	static const double G_solar_system ;
	
	inline static double dsq(double x0,double y0,double z0,double x1,double y1,double z1) {return sqr(x0-x1) + sqr(y0-y1) + sqr(z0-z1);}
	
	/**
	 *  Calculate kinetic energy for a system of particles
	 */
	double get_kinetic_energy(unique_ptr<Particle[]> &particles, int n);

	/**
	 *  Calculate gravitational potential energy for a system of particles
	 */
	double get_potential_energy(unique_ptr<Particle[]> &particles, int n,const double G,const double a);


	/**
	 * Find linear momentum of a set of particles
	 */
	void get_momentum(unique_ptr<Particle[]> &particles, int n,double& px,double& py,double &pz);

	/**
	 * Find angular momentum of a set of particles
	 */
	void get_angular_momentum(unique_ptr<Particle[]> &particles, int n,double& lx,double& ly,double &lz);

	/**
	 * Find centre of mass of a set of particles.
	 */
	void get_centre_of_mass(unique_ptr<Particle[]> &particles, int n,double& x0,double& y0,double &z0);

};






#endif  //_PHYSICS_HPP

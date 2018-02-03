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
 
#ifndef _PARTICLE_H
#define _PARTICLE_H
#include <vector>
#include <cmath>
#include <math.h>
#include <stdexcept>
#include "utils.h"
#include <iostream>

/**
 * A Particle represents one of the bodies whose motion is being simulated.
 * It is just a passive container for data
 */
class Particle {
  public:
    /**
     * Create particle and initialize position, mass, and velocity. 
	 * Force calculation of acceleration by setting it to NAN.
     */
	Particle(double x,	double y, double z, 
		double vx, 	double vy, double vz,
		double m)
		: _x(x),_y(y),_z(z),
		_vx(vx), _vy(vy), _vz(vz),_m(m),
		_ax(NAN), _ay(NAN), _az(NAN){
			if (std::isnan(x) || std::isnan(y) || std::isnan(z))
				throw std::logic_error("Attempt to create pos from to NAN");				
		}
	
	virtual ~Particle() {
	}

	/**
	 *  Accessor for current position
	 */
	void getPos(double& x,	double& y, double& z) {x=_x;y=_y;z=_z;}
	
	/**
	 *  Set current position
	 */
	void setPos(double x, double y, double z) {
		if (std::isnan(x) || std::isnan(y) || std::isnan(z))
			throw std::logic_error("Attempt to setPos to NAN");
		_x=x;
		_y=y;
		_z=z;}
	
	/**
	 *  Accessor for current velocity
	 */
	void getVel(double& vx,	double& vy, double& vz) {vx=_vx;vy=_vy;vz=_vz;}
	
	/**
	 *  Set current velocity
	 */
	void setVel(double vx,	double vy, double vz) {_vx=vx;_vy=vy;_vz=vz;}
	
	/**
	 *  Accessor for current acceleration
	 */
	void getAcc(double& ax,	double& ay, double& az) {ax=_ax;ay=_ay;az=_az;}
	
	/**
	 *  Set current acceleration
	 */
	void setAcc(double ax,	double ay, double az) {_ax=ax;_ay=ay;_az=az;}
	
	/**
	 *  Set current acceleration
	 */
	void resetAcc() {_ax=0;_ay=0;_az=0;}
	
	/**
	 *  Set current acceleration
	 */
	void accumulateAcc(double ax,	double ay, double az) {_ax+=ax;_ay+=ay;_az+=az;}
	
	/**
	 *  Accessor for mass
	 */
	double getMass() {return _m;}
	
	inline double get_distance_sq(Particle*other)  {return sqr(_x-other->_x)+sqr(_y-other->_y)+sqr(_z-other->_z);}
	
  private:
	/**
	 * Position
	 */
	double _x;
	double _y;
	double _z;
	
	/**
	 * Velocity
	 */
	double _vx;
	double _vy;
	double _vz;
	
	/**
	 * Mass
	 */
	double _m;
	
	/**
	 * Acceleration
	 */
	double _ax;
	double _ay;
	double _az;
};

#endif
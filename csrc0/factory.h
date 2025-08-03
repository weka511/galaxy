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
 
#ifndef _FACTORY_H
#define _FACTORY_H

#include <vector>
#include "particle.h"
using namespace std;
/**
 * Factory for instantiating configurations of particles
 */
class Factory {
  public:
  	/**
	 * Create a selection of particles
	 */
	virtual vector<Particle*>  create()=0;
	
	virtual ~Factory() {;}
	
	double zero_centre_mass(vector<Particle*> particles);
	
	void zero_linear_momentum(vector<Particle*> particles,double total_mass);
	
	void setNumBodies(int numbodies) {_numbodies=numbodies;}
	
	int getNumbodies() {return _numbodies;}
	
  protected:
    Factory (int numbodies) : _numbodies(numbodies){;}
	
  private:	
	/**
	 * Number of bodies 
	 */
	 int    	_numbodies;
};


#endif

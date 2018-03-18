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
 *
 * Parse specification for starting configuration
 */
 
#ifndef _PARTICLE_FACTORY_H
#define _PARTICLE_FACTORY_H

#include <string>
#include <vector>
#include "particle.h"
#include "configs.h"
#include "tinyxml2.h"

class ParticleFactory {
  public:
	ParticleFactory(Factory * factory) : _factory(factory) {}
	std::vector<Particle*> create(std::string name);
  private:
	Factory * _factory;
	int _get_attr(tinyxml2::XMLElement * element,std::string name,const int default_value=-1);
	void _get_attr(tinyxml2::XMLElement * element,std::string name,double &x,double &y,double &z);
};

#endif

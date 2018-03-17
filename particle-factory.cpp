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
 
#include <cstring>
#include <iostream>
#include "particle-factory.h"
#include "tinyxml2.h"

std::vector<Particle*> ParticleFactory::create(std::string name){
	std::vector<Particle*> product;
	tinyxml2::XMLDocument doc;
    doc.LoadFile( name.c_str() );
	doc.Print();
	tinyxml2::XMLElement * system=doc.RootElement();
	std::cout<<system->Name()<<":" << system->Attribute("name")<<std::endl;
	for (tinyxml2::XMLElement * element=system->FirstChildElement();
			element!=NULL;
			element=element->NextSiblingElement()){
		std::cout<<element->Name()<<":" << element->Attribute("name")<<std::endl;
	}
	return product;
}

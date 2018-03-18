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

std::vector<Particle*> ParticleFactory::create(std::string name){
	std::cout << __FILE__ <<", " <<__LINE__<<std::endl; 
	std::vector<Particle*> product;
	tinyxml2::XMLDocument doc;
    doc.LoadFile( name.c_str() );
	doc.Print();
	std::cout << __FILE__ <<", " <<__LINE__<<std::endl; 
	tinyxml2::XMLElement * system=doc.RootElement();
	std::cout<<system->Name()<<":" << system->Attribute("name")<<std::endl;
	for (tinyxml2::XMLElement * element=system->FirstChildElement();
			element!=NULL;
			element=element->NextSiblingElement()){
		const int numbodies=_get_attr(element,"numbodies");
		double x,y,z;
		_get_attr(element,"pos",x,y,z);
		double vx,vy,vz;
		_get_attr(element,"vel",vx,vy,vz);
		std::cout << numbodies << ", " << x << ", " << y<< ", " << z<< ", " << vx << ", " << vy << ", " << vz <<std::endl;
		// std::cout<<element->Name()<<":" << element->Attribute("name")<<std::endl;
		// std::cout<<element->Name()<<":" << element->Attribute("numbodies")<<std::endl;
		// std::cout<<element->Name()<<":" << element->Attribute("pos")<<std::endl;
		// std::cout<<element->Name()<<":" << element->Attribute("vel")<<std::endl;
	}
	product=_factory->create();
	return product;
}

int ParticleFactory::_get_attr(tinyxml2::XMLElement * element,std::string name,const int default_value){
	char * key=const_cast<char*>(name.c_str());
	const char * attr=element->Attribute(key);
	return attr==NULL? default_value : atoi(attr);		
}

void ParticleFactory::_get_attr(tinyxml2::XMLElement * element,std::string name,double &x,double &y,double &z){
	char * key=const_cast<char*>(name.c_str());
	std::string attr=element->Attribute(key);
	std::size_t found1 = attr.find(',');
	std::size_t found2 = attr.find(',',found1+1);
	std::cout << attr.substr(0,found1) << "|" << attr.substr(found1+1,found2-found1-1) << "|" << attr.substr(found2+1) << std::endl;
	x=atof(attr.substr(0,found1).c_str());
	y=atof(attr.substr(found1+1,found2-found1-1).c_str());
	z=atof(attr.substr(found2+1).c_str());
}

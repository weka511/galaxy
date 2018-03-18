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
#include "physics.h"

std::vector<Particle*> ParticleFactory::create(std::string name){
	std::cout << __FILE__ <<", " <<__LINE__<<std::endl; 
	std::vector<Particle*> product;
	tinyxml2::XMLDocument doc;
    doc.LoadFile( name.c_str() );

	tinyxml2::XMLElement * system=doc.RootElement();

	product.reserve(_count_bodies(system));
	for (tinyxml2::XMLElement * element=system->FirstChildElement();
			element!=NULL;
			element=element->NextSiblingElement()){
		const int numbodies=_get_attr(element,"numbodies");
		double x,y,z;
		_get_attr(element,"pos",x,y,z);
		double vx,vy,vz;
		_get_attr(element,"vel",vx,vy,vz);
//		_factory->setNumBodies(numbodies);   FIXME
		std::vector<Particle*> cluster=_factory->create();
		_fix_centre_mass_and_linear_momentum(cluster,x,y,x,vx,vy,vz);
		product.insert( product.end(), cluster.begin(), cluster.end() );
	}

	return product;
}

int ParticleFactory::_count_bodies(tinyxml2::XMLElement *system) {
	int count=0;
	for (tinyxml2::XMLElement * element=system->FirstChildElement();
		element!=NULL;
		element=element->NextSiblingElement()){
		const int numbodies=_get_attr(element,"numbodies");
		count+=numbodies;
	}
	return count;
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
	x=atof(attr.substr(0,found1).c_str());
	std::size_t found2 = attr.find(',',found1+1);
	y=atof(attr.substr(found1+1,found2-found1-1).c_str());
	z=atof(attr.substr(found2+1).c_str());
}

void ParticleFactory::_fix_centre_mass_and_linear_momentum(std::vector<Particle*> particles,
	double xc, double yc, double zc,double vxc, double vyc, double vzc) {

	double x0,y0,z0;
	get_centre_of_mass(particles,x0,y0,z0);

	double total_mass=0;
	for (std::vector<Particle*>::iterator it = particles.begin() ; it != particles.end(); ++it) {
		double x,y,z;
		(*it)->getPos(x,y,z);
		x+=(xc-x0);y+=(yc-y0);z+=(zc-z0);
		(*it)->setPos(x,y,z);
		total_mass+=(*it)->getMass();
	}
	
	double px0,py0,pz0;
	get_momentum(particles,px0,py0,pz0);
	for (std::vector<Particle*>::iterator it = particles.begin() ; it != particles.end(); ++it) {	
		double vx,vy,vz;
		(*it)->getVel(vx,vy,vz);
		vx+=(vxc-px0/total_mass);vy+=(vyc-py0/total_mass);vz+=(vzc-pz0/total_mass);
		(*it)->setVel(vx,vy,vz);
	}
 }

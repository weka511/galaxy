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
 
#include "configs.h"
#include "physics.h"
#include "spdlog/spdlog.h"

namespace spd = spdlog;

 /**
  * Create all bodies needed at start of run
  */
 std::vector<Particle*>  Configuration::createParticles(int numbodies,double inivel,double ini_radius,double mass ){
	spdlog::get("galaxy")->info("{0} {1}: initializing {2} bodies, radius={3}",__FILE__,__LINE__,numbodies,ini_radius);

	std::vector<std::vector<double>> positions=direct_sphere(3,numbodies);
	std::vector<Particle*> product;
	
	for (std::vector<std::vector<double>>::iterator it = positions.begin() ; it != positions.end(); ++it) {
        const double x     = (*it)[0] * ini_radius;
        const double y     = (*it)[1] * ini_radius ;
		const double z     = flat_flag==0 ? (*it)[2] * ini_radius :0;
        const double rnorm = std::sqrt(sqr(x)+sqr(y)+sqr(z));
		const double v     = 2*M_PI/std::sqrt(rnorm*rnorm*rnorm);
        const double vx    = -y *v;
        const double vy    =  x *v;
		const double vz    = flat_flag==0 ? (std::rand()%2==0 ? 0.1*vx : -0.1*vx) : 0;
        product.push_back( new Particle( x, y, z, vx, vy,vz, mass) );
    }
	zero_centre_mass_and_linear_momentum(product);
	spdlog::get("galaxy")->info("{0} {1}: initialized {2} bodies.",__FILE__,__LINE__,numbodies);
	return product;
 }
 
 
/**
 * Set centre of mass and total linear momentum to (0,0,0) by adjusting
 * the position and velocity of each point
 */
 void Configuration::zero_centre_mass_and_linear_momentum(std::vector<Particle*> particles) {
	double x0,y0,z0;
	get_centre_of_mass(particles,x0,y0,z0);

	double total_mass=0;
	for (std::vector<Particle*>::iterator it = particles.begin() ; it != particles.end(); ++it) {
		double x,y,z;
		(*it)->getPos(x,y,z);
		x-=x0;y-=y0;z-=z0;
		(*it)->setPos(x,y,z);
		total_mass+=(*it)->getMass();
	}
	
	double px0,py0,pz0;
	get_momentum(particles,px0,py0,pz0);
	for (std::vector<Particle*>::iterator it = particles.begin() ; it != particles.end(); ++it) {	
		double vx,vy,vz;
		(*it)->getVel(vx,vy,vz);
		vx-=px0/total_mass;vy-=py0/total_mass;vz-=pz0/total_mass;
		(*it)->setVel(vx,vy,vz);
	}
 }
 
 int Configuration::get_max_digits_config(){
	int max_digits_config=5;
	const int max_imgs=std::ceil(((double)max_iter)/img_iter);
	return std::max((int)std::ceil(std::log10(max_imgs)),max_digits_config);
 }
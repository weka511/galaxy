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

#include <assert.h> 
#include <cmath>
#include <iostream>
#include "physics.h"
#include "utils.h"

void get_acceleration(std::vector<Particle*> particles,double G) {
	for (int i=1;i<particles.size();i++)
		for (int j=0;j<i;j++)
			get_acceleration_between_pair(particles[i],particles[j],G);
}

const double epsilon=1e-8;  // tolerance for checking Newton's 3rd law

void get_acceleration_between_pair(Particle* p1,Particle* p2,double G) {
	double x1,y1,z1;
	p1->getPos(x1,y1,z1);
	double x2,y2,z2;
	p2->getPos(x2,y2,z2);
	const double distance= sqrt(sqr(x1-x2)+sqr(y1-y2)+sqr(z1-z2));
	double d_factor=1/(distance*distance*distance);
	const double acc_x1 =  p2->getMass()*G*(x2-x1)*d_factor;
	const double acc_y1 =  p2->getMass()*G*(y2-y1)*d_factor;
	const double acc_z1 =  p2->getMass()*G*(z2-z1)*d_factor;
	p1->setAcc(acc_x1,acc_y1,acc_z1);
	const double acc_x2 =  p1->getMass()*G*(x1-x2)*d_factor;
	const double acc_y2 =  p1->getMass()*G*(y1-y2)*d_factor;
	const double acc_z2 =  p1->getMass()*G*(z1-z2)*d_factor;
	p2->setAcc(acc_x2,acc_y2,acc_z2);
	// check Newton's third law
	assert( std::fabs(p1->getMass()*acc_x1+p2->getMass()*acc_x2)<=epsilon*(std::fabs(p1->getMass()*acc_x1)+std::fabs(p2->getMass()*acc_x2)));
	assert( std::fabs(p1->getMass()*acc_y1+p2->getMass()*acc_y2)<=epsilon*(std::fabs(p1->getMass()*acc_y1)+std::fabs(p2->getMass()*acc_y2)));
	assert( std::fabs(p1->getMass()*acc_z1+p2->getMass()*acc_z2)<=epsilon*(std::fabs(p1->getMass()*acc_z1)+std::fabs(p2->getMass()*acc_z2)));
}
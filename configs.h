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
 
#ifndef _CONFIGS_H
#define _CONFIGS_H

#include <iostream>
#include <fstream>
#include <vector>
#include "particle.h"

 /**
  * Create all bodies needed at start of run
  */
std::vector<Particle*>  createParticles(int numbodies,double inivel,double ini_radius,double mass,std::ofstream& logfile,int flat_flag);

/**
 * Set centre of mass and total linear momentum to (0,0,0)
 */
void zero_centre_mass_and_linear_momentum(std::vector<Particle*> particles);

#endif
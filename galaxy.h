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

#ifndef _GALAXY_H
#define _GALAXY_H
#include <limits>

#include "particle.h"

/**
 *  Process command line options. Returns `true` iff execution is to continue.
 */
bool extract_options(int argc, char **argv);

 

/**
  * Generate help text
  */
void help();
 
 /**
  * Used after iteration to write out data
  */
 bool report_all(std::vector<Particle*> particles,int iter);
 
 /**
  * Write out configuration
  */
 void report_configuration(std::vector<Particle*> particles,int iter);
 
 /**
  * Write out energy and other conserved quantities
  */
void report_energy(std::vector<Particle*> particles,int iter);

/**
 * Parse numeric command line parameter and validate:
 * 1. any extraneous parameters
 * 2. are value within range?
 */
int get_number(std::string name, char * param, int high=std::numeric_limits<int>::max(),int low=0);

/**
 * Parse numeric command line parameter and validate:
 * 1. any extraneous parameters
 * 2. are value within range?
 */
double get_double(std::string name,char * param, double high=std::numeric_limits<double>::max(),double low=0);

#endif

 
 
 
 
 
 
 

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

#include "particle.h"

/**
 *  Process command line options. Returns `true` iff execution is to continue.
 */
bool extract_options(int argc, char **argv);

 /**
  * Create all bodies needed at start of run
  */
std::vector<Particle*>  createParticles(int numbodies,double inivel,double ini_radius,double mass);

/**
  * Generate help text
  */
void help();
 
 bool report_all(std::vector<Particle*> particles,int iter);
 
 /**
  * Write out configuration
  */
 bool report_configuration(std::vector<Particle*> particles,int iter);
 
void report_energy(std::vector<Particle*> particles,int iter);
 
 //----------------------------------------------------------------------------------------------------
 
 /**
  * Execute simulation
  */
// void simulate(int start_iter,int max_iter,std::vector<Body*> bodies, double theta, double G, double dt, int img_iter,std::string path,std::string config_file_name,int check_energy);
 
 
 /**
  * Restore configuration from saved file
  */
// bool restore_config(std::string path,std::string name,std::vector<Body*>& bodies, int &iter, double &theta, double &G, double& dt);

 /**
  * Restore value stored by encode
  */
// void save_config( std::vector<Body*>& bodies, int iter, double theta, double G, double dt, std::string path,std::string name="config.txt");

/**
 * Retrieve position, mass, and velocities stored for one Body
 */
// Body * extract_body(std::string line);



#endif

 
 
 
 
 
 
 

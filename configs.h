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
#include "physics.h"


class Configuration {
  public:
   /**
	* Create all bodies needed at start of run
	*/
	std::vector<Particle*>  createParticles();

	/**
	* Set centre of mass and total linear momentum to (0,0,0)
	*/
	void zero_centre_mass_and_linear_momentum(std::vector<Particle*> particles);
	
	int get_max_digits_config();
	
	void save_config( std::vector<Particle*>& particles,int iter);
	
	Particle * extract_particle(std::string line);
	
	bool restore_config(std::vector<Particle*>& bodies,int& iter);
	
	int flat_flag;

	/**
	 * Version number for configuration records
	 */
	std::string config_version="0.0";
	
	/**
	 *  Theta-criterion of the Barnes-Hut algorithm.
	 *  I had to move this outside `main` so the lambda in `run_verlet` would compile.
	 */
	double theta = 0.5;

	/**
	* The "gravitational constant" is chosen so as to get a pleasant output.
	*/
	double G = G_solar_system;
		
	/**
	 * Mass of a body.
	 */
	double mass = 1.0;

	/**
	 * Initially, the bodies are distributed inside a circle of radius ini_radius.
	 */
	double ini_radius = 1.0;

	/** 
	 * Initial maximum velocity of the bodies.
	 */
	double inivel = 0.1;

	/**
	 * Discrete time step.
	 */
	double dt = 1.e-3;

	/**
	 * Number of bodies
	 */
	int numbodies = 1000;

	/**
	 * Number of time-iterations executed by the program.
	 */
	int max_iter = 10000;

	/**
	 * Frequency at which configuration records are written.
	 */
	int img_iter = 20;

	 /**
	  * Frequency for checking total energy
	  */
	int check_energy = -1;

	/**
	 * File Name for configuration records
	 */
	std::string config_file_name="config.txt";

	/**
	 * Folder for configuration records
	 */
	std::string path = "./configs";



	double  E0 =0; // initial energy

	double maximum_energy_error=0;  //largest discrepancy in energy

	double softening_length=0.0001;  // Used to handle collisions
	
};


#endif
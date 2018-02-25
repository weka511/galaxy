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
#include <random>

#include "particle.h"
#include "physics.h"

int get_resume_flag();

class Configuration {
  public:
  /**
   * Used to select model for initial conditions, e.g. Plummer, King, etc
   */
    enum Model {
		Plummer     // at present Plummer is the only model
	};
	
	Configuration();
	
	bool extract_options(int argc, char **argv);
	
   /**
	* Create all bodies needed at start of run
	*/
	std::vector<Particle*>  createParticles();
	
	/**
	* Set centre of mass and total linear momentum to (0,0,0)
	*/
	void zero_centre_mass_and_linear_momentum(std::vector<Particle*> particles,int iter);
	
	int get_max_digits_config();
	
	/**
	 * Save configuration for restart.
	 */
	void save_config( std::vector<Particle*>& particles,int iter);
	
	/**
	 * Used to retrieve one saved particle. 
	 */
	Particle * extract_particle(std::string line);
	
	/**
	 * Restore saved configuration
	 */
	bool restore_config(std::vector<Particle*>& bodies,int& iter);
	

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
	* The gravitational constant is 1 in Heggie units - https://link.springer.com/chapter/10.1007%2FBFb0116419
	*/
	double G = 1.0;
		
	/**
	 * The Mass of the system is 1 in Heggie units - https://link.springer.com/chapter/10.1007%2FBFb0116419
	 */
	double M = 1.0;

	/**
	 * Initially, the bodies are distributed inside a circle of radius ini_radius.
	 */
	double ini_radius = 1.0;

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

	Model model=Plummer;

	double  E0 =0; // initial energy

	double maximum_energy_error=0;  //largest discrepancy in energy

	double softening_length=1;  // Used to handle collisions
	
	int needToZero = 2;
	
	unsigned int seed=time(NULL);
	
  private:
	
	/**
	 *   Create particles satisfying Plummer distribution, following the derivateion in Hut & Makino
	 *   http://www.artcompsci.org/kali/vol/plummer/volume9.pdf
	 */
	std::vector<Particle*>  createPlummerDistribution( ); 
	
	std::default_random_engine generator;
	std::uniform_real_distribution<double> uniform_distribution_theta;
	std::uniform_real_distribution<double> uniform_distribution_phi;
	std::uniform_real_distribution<double> uniform_distribution_radius;
	std::uniform_real_distribution<double> uniform_distribution_x;
	std::uniform_real_distribution<double> uniform_distribution_y;
	
	/**
	 * Convert a scalar, r, into a vector with the same length, and a random orientation
	 */
	void randomize_theta_phi(const double r,double & x,double & y,double& z) ;
	
	/**
	 * Sample velocities, ensuring that the initial velocity of any
	 * star does not exceed escape velocity
	 */
	double sample_velocity(const double radius);
	
	/**
	 * Generate help text
	 */
	void help();
	
};

 
#endif
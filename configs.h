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
#include "plummer.h"

int get_resume_flag();

class Configuration {
  public:
  /**
   * Used to select model for initial conditions, e.g. Plummer, King, etc
   */
    enum Model {
		Plummer     // at present Plummer is the only model
	};
	
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
	 * Write out configuration
	 */
	void report_configuration(std::vector<Particle*> particles,int iter);
	 
	std::string get_config_file_name() {return config_file_name;}

	std::string get_path() {return path;};
	 
	/**
	 * Version number for configuration records
	 */
	std::string config_version="0.0";
	
	inline double get_softening_length() {return softening_length;}
	
	inline double getG() {return G;}

	/**
	 * Discrete time step.
	 */	
	inline double get_dt() { return dt;}
	
	inline double get_max_iter() {return max_iter;}

	/**
	 *  Theta-criterion of the Barnes-Hut algorithm.
	 */
	inline double get_theta() {return theta;}
	
	inline bool should_check_energy(const int iter) {return check_energy>0 &&iter%check_energy==0;}
	
 private:
	
	/**
	 *  Theta-criterion of the Barnes-Hut algorithm.
	 */
	double theta = 0.5;
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
	 * File Name for configuration records
	 */
	std::string config_file_name="config.txt";

	/**
	 * Folder for configuration records
	 */
	std::string path = "./configs";

	Model model=Plummer;

	double softening_length=1;  // Used to handle collisions
	
	int needToZero = 2;
	
	unsigned int seed=time(NULL);
	
  	 /**
	  * Frequency for checking total energy
	  */
	int check_energy = -1;
	
	Factory * createFactory();
	
	/**
	 * Generate help text
	 */
	void help();
	
};

 
#endif
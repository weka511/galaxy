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
	 
	std::string get_config_file_name() {return _config_file_name;}

	std::string get_path() {return _path;};
	
	inline double get_a() {return _a;}
	
	inline double getG() {return _G;}

	/**
	 * Discrete time step.
	 */	
	inline double get_dt() { return _dt;}
	
	inline double get_max_iter() {return _max_iter;}

	/**
	 *  Theta-criterion of the Barnes-Hut algorithm.
	 */
	inline double get_theta() {return _theta;}
	
	inline bool should_check_energy(const int iter) {return _check_energy>0 &&iter%_check_energy==0;}
	
	inline int get_n_threads() {return _n_threads;}
	
 private:
 
	/**
	 *  Instantiate Factory for setting up particles. This depends on the model.
	 */
	Factory * _createFactory();
	
	/**
	 * Generate help text
	 */
	void _help();
	
	/**
	 * Version number for configuration records
	 */
	std::string _config_version   = "0.0";
	
	/**
	 *  Theta-criterion of the Barnes-Hut algorithm.
	 */
	double _theta                 = 0.5;
	
	/**
	 * Discrete time step.
	 */
	double _dt                    = 1.e-2;

	/**
	 * Number of bodies 
	 */
	int _numbodies                = 1000;

	/**
	 * Number of time-iterations executed by the program.
	 */
	int _max_iter                 = 10000;

	/**
	 * Frequency at which configuration records are written.
	 */
	int _img_iter                 = 20;
	/**
	* The gravitational constant is 1 in Heggie units - https://link.springer.com/chapter/10.1007%2FBFb0116419
	*/
	double _G                     = 1.0;
		
	/**
	 * The Mass of the system is 1 in Heggie units - https://link.springer.com/chapter/10.1007%2FBFb0116419
	 */
	double _M                     = 1.0;

	/**
	 * Initially, the bodies are distributed inside a circle of radius ini_radius.
	 */
	double _ini_radius            = 1.0;
	
	/**
	 * File Name for configuration records
	 */
	std::string _config_file_name = "config.txt";

	/**
	 * Folder for configuration records
	 */
	std::string _path             = "./configs";

	Model _model                  = Plummer;

	double _a                     = 1;  // Used to handle collisions
	
	int _needToZero               = 2;
	
	unsigned int _seed            = time(NULL);
	
  	 /**
	  * Frequency for checking total energy
	  */
	int _check_energy             = -1;
	
	int _n_threads					= 0;
	
	std::string _initial_configuration_file ="";
	
};

 
#endif
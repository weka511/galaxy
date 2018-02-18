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
 
#include <chrono>
#include <ctime>
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>
#include <ostream>
#include <sstream>
#include <getopt.h>
#include <algorithm>
#include <stdexcept>
#include <stdlib.h> 
#include <time.h> 
#include "barnes-hut.h"
#include "configs.h"
#include "galaxy.h"
#include "center-of-mass.h"
#include "treecode.h"
#include "utils.h"
#include "verlet.h"
#include "physics.h"
#include "spdlog/spdlog.h"

namespace spd = spdlog;

Configuration configuration;

static int resume_flag = 0;
 
/**
 *  Long version of command line options.
 */
struct option long_options[] = {
	{"flat", 			no_argument,       	&configuration.flat_flag, 	1},
	{"config",  		required_argument,	0, 				'c'},
	{"dt",  			required_argument,	0, 				'd'},
	{"check_energy",  	required_argument,	0, 				'e'},
	{"G",  				required_argument, 	0, 				'G'},
    {"help",  			no_argument, 		0, 				'h'},
	{"img_iter",		required_argument, 	0, 				'i'},
	{"max_iter",  		required_argument, 	0, 				'm'},
	{"numbodies",  		required_argument, 	0, 				'n'},
	{"path",  			required_argument, 	0, 				'p'},
	{"plummer",  		no_argument, 		0, 				'l'},
	{"ini_radius",  	required_argument, 	0, 				'r'},
	{"mass",  			required_argument, 	0, 				's'},
	{"resume", 			no_argument,       	&resume_flag, 	1},
	{"theta",  			required_argument, 	0, 				't'},
	{"inivel",  		required_argument, 	0, 				'v'},
	{"seed",  			no_argument, 		0, 				'S'},
	{"soften",  		required_argument, 	0, 				'f'},
	{"zero",  			required_argument, 	0, 				'z'},
	{0, 				0, 					0, 				0}
};	



/**
 * Main program. Parse command line options, create bodies, then run simulation.
 */
int main(int argc, char **argv) {
	try {
		auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("./logs/logfile", 23, 59);
		auto logger = std::make_shared<spdlog::logger>("galaxy", daily_sink);
		logger->set_level(spdlog::level::info);
		spdlog::register_logger(logger);
			std::srand(time(NULL));
		auto start = std::chrono::system_clock::now();
		std::time_t start_time = std::chrono::system_clock::to_time_t(start);


		if (extract_options(argc,argv)) {
			std::vector<Particle*> particles;
			int start_iterations=0;
			if (resume_flag) {
				if (configuration.restore_config(particles,start_iterations))
					logger->info("Restarted from {0} {1} at {2}",configuration.path,configuration.config_file_name,start_iterations);
				else {
					particles = configuration.createParticles(  );
					start_iterations=0;
					logger->info("Failed to restart from {0} {1}",configuration.path,configuration.config_file_name);
				}
			} else {
				std::system("rm configs/*");  // Issue #5 - remove old config files
				particles = configuration.createParticles(  );
			}
	
			report_all(particles,start_iterations);
			configuration.E0 = get_energy(particles,configuration.G,configuration.softening_length);

			run_verlet([](	std::vector<Particle*> particles)->void{get_acceleration(particles,configuration.theta,configuration.G,configuration.softening_length);},
						configuration.max_iter,
						configuration.dt,
						particles,
						&report_all,
						start_iterations);
			if (configuration.check_energy>0 )
				logger->info("Energy Error={0}, {1}%",
							configuration.maximum_energy_error,
							(int)(100*configuration.maximum_energy_error/abs(configuration.E0)));
						
			auto end = std::chrono::system_clock::now();
		 
			std::chrono::duration<double> elapsed_seconds = end-start;
			std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		 
			logger->info("finished computation at {0}, elapsed time: {1} seconds", std::ctime(&end_time), elapsed_seconds.count());
					  
			return EXIT_SUCCESS;
		} else {
			logger->error("Terminating: failed to parse command line parameters.");
			return EXIT_FAILURE;
		}
		
	} catch (const std::exception& ex){
		std::cerr << argv[0]<< " halted following an exception: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
}

/**
 *  Process command line options. Returns `true` iff execution is to continue.
 */
bool extract_options(int argc, char **argv) {
	auto logger=spdlog::get("galaxy");
	int option_index = 0;
	int c;

	while ((c = getopt_long (argc, argv, "c:d:e:G:hi:lm:n:p:r:Ss:t:v:f:",long_options, &option_index)) != -1)
		switch (c){
			case 'c':
				configuration.config_file_name=optarg;
				logger->info("Configuration File={0}",configuration.config_file_name);
				break;
			
			case 'd':
				configuration.dt=get_double("dt",optarg,0.5);
				break;
			
			case 'e':
				configuration.check_energy=get_number("check_energy",optarg);
				break;
			
			case 'f':
				configuration.softening_length=get_double("Softening Length",optarg);
				break;
				
			case 'G':
				configuration.G=get_double("G",optarg);
			
			case 'h':
				help( );
				return false;
			
			case 'i':
				configuration.img_iter=get_number("Frequency at which configs are written",optarg);
				break;
				
			case 'l':
				configuration.model = Configuration::Plummer;
				logger->info("Plummer Model");
				break;
			
			case 'm':
				configuration.max_iter=get_number("Number of iterations",optarg);
				break;
			
			case 'n':
				configuration.numbodies=get_number("Number of bodies",optarg);
				break;
			
			case 'p':
				configuration.path=optarg;
				logger->info("Path={0}",configuration.path);
				break;
			
			case 'r':
				configuration.ini_radius= get_double("Initial radius",optarg);
				break;
			
			case 'S':
				logger->info("Seed random number generator");
				std::srand(1);
				break;
			
			case 's':
				configuration.mass=get_double("mass",optarg);
				break;
				
			case 't':
				configuration.theta=get_double("Theta",optarg);
				break;
			
			case 'v':
				configuration.inivel=get_double("Velocity",optarg);
				break;
			
			case 'z':
				configuration.needToZero=get_number("Need to zero",optarg,3,-1);
				break;
				
			case '?':
				return false;
		}
		
	if (!ends_with(configuration.path,"/"))
		configuration.path.append("/");

	return true;
}	


/**
 * Used after iteration to write out data
*/
bool report_all(std::vector<Particle*> particles,int iter){
	report_energy(particles,iter);
	report_configuration(particles,iter);
	return !killed();
}

/**
  * Write out energy and other conserved quantities
  */
void report_energy(std::vector<Particle*> particles,int iter) {
	if (configuration.check_energy>0 ) {
		auto logger=spdlog::get("galaxy");
		configuration.zero_centre_mass_and_linear_momentum(particles,iter);
		const double E=get_energy(particles,configuration.G,configuration.softening_length);
		if (abs(E-configuration.E0)>configuration.maximum_energy_error)	
			configuration.maximum_energy_error=abs(E-configuration.E0);
		
		if (iter%configuration.check_energy==0) {
			logger->info("Conserved quantities for iteration {0}", iter);
			double x0,y0,z0;
			get_centre_of_mass(particles,x0,y0,z0);
			logger->info("Centre of mass=({0},{1},{2})", x0,y0,z0);	
			
			double px,py,pz;
			get_momentum(particles,px,py,pz);
			logger->info("Momentum=({0},{1},{2})",px,py,pz);
			
			double lx,ly,lz;
			get_angular_momentum(particles,lx,ly,lz);
			logger->info("Angular momentum=({0},{1},{2})",lx,ly,lz);
			
			logger->info("Energy {0}", E);
		}
	}
}

/**
  * Write out configuration
  */
void report_configuration(std::vector<Particle*> particles,int iter) {
	if (iter%configuration.img_iter==0) {
		configuration.zero_centre_mass_and_linear_momentum(particles,iter);
		std::cout << "Writing configuration for iteration " << iter << std::endl;
		std::stringstream file_name;
		file_name << configuration.path<< "bodies" << std::setw(configuration.get_max_digits_config()) << std::setfill('0') <<iter/configuration.img_iter << ".csv";
		std::ofstream ofile(file_name.str().c_str());
		for (std::vector<Particle*>::iterator it = particles.begin() ; it != particles.end(); ++it) {
			double x,y,z;
			(*it)->getPos(x,y,z);
			ofile<<x <<"," <<y <<","<<z <<std::endl;
		}
		ofile.close();
		configuration.save_config(particles,iter);
	}

}


 



/**
  * Generate help text
  */
void help() {
	std::cout << "Galaxy Simulator based on Barnes Hut algorithm." << std::endl<<std::endl;
	std::cout << "Parameters, showing default values" <<std::endl;
	std::cout << "\t-c,--config\t\tConfiguration file [" << configuration.config_file_name<<"]"<< std::endl;
	std::cout << "\t-d,--dt\t\tTime Step for Integration [" << configuration.dt<<"]"<< std::endl;
	std::cout << "\t-e,--check_energy\tCheck total energy every `check_energy` iterations[don't check]"<< std::endl;
	std::cout << "\t--flat\t\tUsed to set z to origin for 3D only"<< std::endl;
	std::cout << "\t-f,--soften\tSoftening Length[" << configuration.softening_length << "]"<<std::endl;
	std::cout << "\t-G,--G\t\tGravitational Constant [" << configuration.G << "]"<<std::endl;
	std::cout << "\t-h,--help\tShow help text" << std::endl;
	std::cout << "\t-i,--img_iter\tFrequency for writing positions [" << configuration.img_iter << "]"<< std::endl;
	std::cout << "\t-l,--plummer\tUse a Plummer model for starting positions and velocities" << std::endl;
	std::cout << "\t-m,--max_iter\tMaximum number of iterations [" << configuration.max_iter << "]"<< std::endl;
	std::cout << "\t-n,--numbodies\tNumber of bodies [" << configuration.numbodies<< "]"<<std::endl;
	std::cout << "\t-p,--path\tPath for writing configurations [" << configuration.path << "]"<< std::endl;
	std::cout << "\t-r,--ini_radius\tInitial Radius [" << configuration.ini_radius << "]"<<std::endl;
	std::cout << "\t--resume\tResume previous run"<<std::endl;
	std::cout << "\t-s,--mass\tMass of bodies [" << configuration.mass << "]"<<std::endl;
	std::cout << "\t-S,--seed\tSeed random number generator"<<std::endl;
	std::cout << "\t-t,--theta\tTheta-criterion of the Barnes-Hut algorithm [" << configuration.theta << "]"<< std::endl;
	std::cout << "\t-v,--inivel\tInitial velocities [" << configuration.inivel << "]"<<std::endl;
	std::cout << "\t-z,--zero\tReset centre of mass and momentum [" << configuration.needToZero << "]"<<std::endl;
}

/**
 * Parse numeric command line parameter and validate:
 * 1. any extraneous parameters
 * 2. are value within range?
 */
double get_double(std::string name, char * param, double high,double low){
	auto logger=spdlog::get("galaxy");
	std::string::size_type sz;	
	const double retval = std::stod (param,&sz);
	if (sz==strlen(param) && low<retval && retval<high){
		logger->info("{0}={1}",name,retval);
		return retval;
	}	else{
		std::stringstream err;
		err<< name <<"=" <<retval<<". Should be in range ("<< low <<"," <<high <<")" <<std::endl;
		throw std::out_of_range(err.str());
	}
} 

/**
 * Parse numeric command line parameter and validate:
 * 1. any extraneous parameters
 * 2. are value within range?
 */
int get_number(std::string name, char * param, int high,int low){
	auto logger=spdlog::get("galaxy");
	std::string::size_type sz;	
	const int retval = std::stoi (param,&sz);
	if (sz==strlen(param) && low<retval && retval<high){
		logger->info("{0}={1}",name,retval);
		return retval;
	}	else{
		std::stringstream err;
		err<< name <<"=" <<retval<<". Should be in range ("<< low <<"," <<high <<")" <<std::endl;
		throw std::out_of_range(err.str());
	}
} 


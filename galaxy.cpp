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
	{"ini_radius",  	required_argument, 	0, 				'r'},
	{"mass",  			required_argument, 	0, 				's'},
	{"resume", 			no_argument,       	&resume_flag, 	1},
	{"theta",  			required_argument, 	0, 				't'},
	{"inivel",  		required_argument, 	0, 				'v'},
	{"soften",  		required_argument, 	0, 				'f'},
	{0, 				0, 					0, 				0}
};	



/**
 * Main program. Parse command line options, create bodies, then run simulation.
 */
int main(int argc, char **argv) {
	auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logfile", 23, 59);
	auto logger = std::make_shared<spdlog::logger>("galaxy", daily_sink);
	logger->set_level(spdlog::level::info);
	spdlog::register_logger(logger);
	
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
		try {
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
			} catch(const std::logic_error& e) {
				std::cerr << e.what() << std::endl;
				logger->info(e.what());
		}
	
		auto end = std::chrono::system_clock::now();
	 
		std::chrono::duration<double> elapsed_seconds = end-start;
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	 
		logger->info("finished computation at {0}, elapsed time: {1} seconds", std::ctime(&end_time), elapsed_seconds.count());
				  
		return EXIT_SUCCESS;
	} else {
		logger->error("Terminating");
		return EXIT_FAILURE;
	}
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
 *  Process command line options. Returns `true` iff execution is to continue.
 */
bool extract_options(int argc, char **argv) {
	int option_index = 0;
	int c;
	
	while ((c = getopt_long (argc, argv, "c:d:e:G:hi:m:n:p:r:Ss:t:v:f:",long_options, &option_index)) != -1)
    switch (c){
		case 'c':{
			std::stringstream param(optarg);
			param>>configuration.config_file_name;
			std::cout<<"Configuration File:="<<configuration.config_file_name<<std::endl;
			break;
		}
		
		case 'd':{
			std::stringstream param(optarg);
			param>>configuration.dt;
			std::cout<<"dt="<<configuration.dt<<std::endl;
			break;
		}
		
		case 'e':{
			std::stringstream param(optarg);
			param>>configuration.check_energy;
			std::cout<<"check_energy="<<configuration.check_energy <<std::endl;
			break;
		}
		
		case 'f':{
			std::stringstream param(optarg);
			param>>configuration.softening_length;
			std::cout<<"Softening Length="<<configuration.softening_length<<std::endl;
			break;
		}
			
		case 'G':{
			std::stringstream param(optarg);
			param>>configuration.G;
			std::cout<<"G="<<configuration.G<<std::endl;
			break;
		}
		
		case 'h':{
			help( );
			return false;
		}
		
		case 'i':{
			std::stringstream param(optarg);
			param>>configuration.img_iter;
			std::cout<<"Frequency at which PNG images are written="<<configuration.img_iter<<std::endl;
			break;
		}
		
		case 'm':{
			std::stringstream param(optarg);
			param>>configuration.max_iter;
			std::cout<<"Number of iterations="<<configuration.max_iter<<std::endl;
			break;
		}
		
		case 'n':{
			std::stringstream param(optarg);
			param>>configuration.numbodies;
			std::cout<<"Number of bodies="<<configuration.numbodies<<std::endl;
			break;
		}
		
		case 'p':{
			std::stringstream param(optarg);
			param>>configuration.path;
			std::cout<<"Path="<<configuration.path<<std::endl;
			break;
		}
		
		case 'r':{
			std::stringstream param(optarg);
			param>>configuration.ini_radius;
			std::cout<<"Initial radius="<<configuration.ini_radius<<std::endl;
			break;
		}
		
		case 'S':{
			std::cout<<"Seed random number generator"<<std::endl;
			std::srand(1);
			break;
		}
		
		case 's':{
			std::stringstream param(optarg);
			param>>configuration.mass;
			std::cout<<"mass="<<configuration.mass<<std::endl;
			break;
		}
			
		case 't':{
			std::stringstream param(optarg);
			param>>configuration.theta;
			std::cout<<"Theta="<<configuration.theta<<std::endl;
			break;
		}
		
		case 'v':{
			std::stringstream param(optarg);
			param>>configuration.inivel;
			std::cout<<"Velocity="<<configuration.inivel<<std::endl;
			break;
		}
		
		case '?':
			return false;
	}
	if (!ends_with(configuration.path,"/"))
		configuration.path.append("/");

	return true;
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
	std::cout << "\t-m,--max_iter\tMaximum number of iterations [" << configuration.max_iter << "]"<< std::endl;
	std::cout << "\t-n,--numbodies\tNumber of bodies [" << configuration.numbodies<< "]"<<std::endl;
	std::cout << "\t-p,--path\tPath for writing configurations [" << configuration.path << "]"<< std::endl;
	std::cout << "\t-r,--ini_radius\tInitial Radius [" << configuration.ini_radius << "]"<<std::endl;
	std::cout << "\t--resume\tResume previous run"<<std::endl;
	std::cout << "\t-s,--mass\tMass of bodies [" << configuration.mass << "]"<<std::endl;
	std::cout << "\t-t,--theta\tTheta-criterion of the Barnes-Hut algorithm [" << configuration.theta << "]"<< std::endl;
	std::cout << "\t-v,--inivel\tInitial velocities [" << configuration.inivel << "]"<<std::endl;
}


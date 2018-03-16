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



/**
 * Main program. Parse command line options, create bodies, then run simulation.
 */
int main(int argc, char **argv) {
	try {
		auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("./logs/logfile", 23, 59);
		auto logger = std::make_shared<spdlog::logger>("galaxy", daily_sink);
		logger->set_level(spdlog::level::info);
		spdlog::register_logger(logger);

		auto start = std::chrono::system_clock::now();
		std::time_t start_time = std::chrono::system_clock::to_time_t(start);

		if (configuration.extract_options(argc,argv)) {

			std::vector<Particle*> particles;
			int start_iterations=0;
			if (get_resume_flag()) {
				if (configuration.restore_config(particles,start_iterations))
					logger->info("Restarted from {0} {1} at {2}",configuration.get_path(),configuration.get_config_file_name(),start_iterations);
				else {
					particles = configuration.createParticles(  );
					start_iterations=0;
					logger->info("Failed to restart from {0} {1}",configuration.get_path(),configuration.get_config_file_name());
				}
			} else {
				std::system("rm configs/*");  // Issue #5 - remove old config files
				particles = configuration.createParticles(  );
				const double T=get_kinetic_energy(particles);
				const double V=get_potential_energy(particles,configuration.getG(),configuration.get_a());
				const double E=T+V;
				logger->info("T={0}, V={1}, E={2}, T/V={3}",T,V,E,T/V);
				std::cout<< "T="<<T <<", V=" << V << ", E=" << E << std::endl;
			}
	
			report_all(particles,start_iterations);

			if (configuration.get_n_threads()==0)
				run_verlet([](	std::vector<Particle*> particles)->void{get_acceleration(
																		particles,
																		configuration.get_theta(),
																		configuration.getG(),
																		configuration.get_a());},
						configuration.get_max_iter(),
						configuration.get_dt(),
						particles,
						&report_all,
						start_iterations);
			else //void (*get_acceleration)(int i, std::vector<Particle*>& particles,Node * root),
				run_verlet(
						[](	std::vector<Particle*> particles)->Node*{create_tree(particles);},
						[](	int i,std::vector<Particle*> particles,Node* root)->
							void{get_acceleration(i,
													particles,
													root,
													configuration.get_theta(),
													configuration.getG(),
													configuration.get_a());},
						configuration.get_max_iter(),
						configuration.get_dt(),
						particles,
						&report_all,
						start_iterations,
						configuration.get_n_threads());
						
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
 * Used after iteration to write out data
*/
bool report_all(std::vector<Particle*> particles,int iter){
	std::cout << __FILE__ << " " <<__LINE__ << " " <<iter <<std::endl;
	report_energy(particles,iter);
	configuration.report_configuration(particles,iter);
	return !killed();
}

/**
  * Write out energy and other conserved quantities
  */
void report_energy(std::vector<Particle*> particles,const int iter) {
	if (configuration.should_check_energy(iter)) {
		auto logger=spdlog::get("galaxy");
		configuration.zero_centre_mass_and_linear_momentum(particles,iter);
		const double T=get_kinetic_energy(particles);
		const double V=get_potential_energy(particles,configuration.getG(),configuration.get_a());
		const double E=T+V;
		logger->info("T={0}, V={1}, E={2}, T/V={3}",T,V,E,T/V);
		std::cout<< "T="<<T <<", V=" << V << ", E=" << E << std::endl;
		
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







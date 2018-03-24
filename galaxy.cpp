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
			else {
				spdlog::set_async_mode(1024);
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
			}		
			auto end = std::chrono::system_clock::now();
		 
			std::chrono::duration<double> elapsed_seconds = end-start;
			std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		 
			logger->info("finished computation at {0}, elapsed time: {1} seconds", std::ctime(&end_time), elapsed_seconds.count());
					  
			return EXIT_SUCCESS;
		} else {
			logger->error("Terminating: failed to parse command line parameters.");
			for (int i=0;i<argc;i++)
				logger->info(argv[i]);
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
	configuration.report_configuration(particles,iter);
	return !killed();
}









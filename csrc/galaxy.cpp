/**
 * Copyright (C) 2025 Simon Crase
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
 *
 * Implementation of the Barnes Hut algorithm to simulate the evolution of a galaxy.
 */

#include <iostream>
#include <chrono>

#include "acceleration.hpp"
#include "barnes-hut.hpp"
#include "integrators.hpp"
#include "logger.hpp"
#include "parameters.hpp"
#include "reporter.hpp"

using namespace std;

int main(int argc, char **argv) {
	auto start = std::chrono::high_resolution_clock::now();
	try {
		unique_ptr<Parameters> parameters=Parameters::get_options(argc, argv);
		Logger::set_paths( parameters->get_base(),  parameters->get_log_path());
		cout << __FILE__ << " " << __LINE__ << " galaxy: " << VERSION << endl;
		LOG2("Galaxy ",VERSION);
		path configuration_file = parameters->get_path();
		configuration_file /=  parameters->get_config_file();
		Configuration configuration(configuration_file);
		AccelerationVisitor calculate_acceleration(configuration, parameters->get_theta(),parameters->get_G(),parameters->get_a());
		Reporter reporter(configuration,parameters->get_base(),parameters->get_path(),"csv","kill",parameters->get_frequency());
		Leapfrog integrator(configuration,  calculate_acceleration,reporter);
		integrator.run(parameters->get_max_iter(),parameters->get_dt());
	}  catch (const exception& e) {
        cerr << __FILE__ << " " << __LINE__ << " Terminating because of errors: "<< endl;
		cerr  << e.what() << endl;
		exit(1);
    }
	auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
	LOG("galaxy Ending");
    return 0;
}

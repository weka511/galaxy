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
#include "barnes-hut.h"
#include "galaxy.h"
#include "center-of-mass.h"
#include "treecode.h"
#include "utils.h"
#include "verlet.h"
#include "physics.h"

/**
 *  Long version of command line options.
 */

static int flat_flag = 0;

static int resume_flag = 0;

struct option long_options[] = {
	{"flat", 			no_argument,       	&flat_flag, 	1},
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
	{0, 				0, 					0, 				0}
};	

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

/**
 * Version number for configuration records
 */
double config_version=0.0;

/**
 * Length of sequence number for config files
 */
int max_digits_config=5;

/**
 * Main program. Parse command line options, create bodies, then run simulation.
 */
int main(int argc, char **argv) {

	if (extract_options(argc,argv)) {
		const int max_imgs=std::ceil(((double)max_iter)/img_iter);
		max_digits_config = std::max((int)std::ceil(std::log10(max_imgs)),max_digits_config);
		std::vector<Particle*> particles = createParticles( numbodies, inivel, ini_radius, mass );
		report_all(particles,0);
		try {
			run_verlet([](	std::vector<Particle*> particles)->void{get_acceleration(particles,theta,G);},
						max_iter,
						dt,
						particles,
						&report_all);
		} catch(const std::logic_error& e) {
			std::cout << e.what() << std::endl;
		}
	}
	
	return EXIT_SUCCESS;
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
	if (check_energy>0 && iter%check_energy==0) {
		std::cout << "Conserved quantities for iteration " << iter << std::endl;
		
		double x0,y0,z0;
		get_centre_of_mass(particles,x0,y0,z0);
		std::cout << "Centre of mass=(" <<x0 << "," <<y0<<"," <<z0<<")"<<std::endl;	
		
		double px,py,pz;
		get_momentum(particles,px,py,pz);
		std::cout << "Momentum=(" <<px << "," <<py<<"," <<pz<<")"<<std::endl;
		
		double lx,ly,lz;
		get_angular_momentum(particles,lx,ly,lz);
		std::cout << "Angular momentum=(" <<lx << "," <<ly<<"," <<lz<<")"<<std::endl;
		
		std::cout << "Energy " << get_energy(particles,G) << std::endl;
	}
}

/**
  * Write out configuration
  */
void report_configuration(std::vector<Particle*> particles,int iter) {
	if (iter%img_iter==0) {
		std::cout << "Writing configuration for iteration " << iter << std::endl;
		std::stringstream file_name;
		file_name << path<< "bodies" << std::setw(max_digits_config) << std::setfill('0') <<iter/img_iter << ".csv";
		std::ofstream ofile(file_name.str().c_str());
		for (std::vector<Particle*>::iterator it = particles.begin() ; it != particles.end(); ++it) {
			double x,y,z;
			(*it)->getPos(x,y,z);
			ofile<<x <<"," <<y <<","<<z <<std::endl;
		}
		ofile.close();
	}

}

 /**
  * Create all bodies needed at start of run
  */
 std::vector<Particle*>  createParticles(int numbodies,double inivel,double ini_radius,double mass ){
	std::cout<<__FILE__ <<", " <<__LINE__<< ": Initializing " << numbodies << " bodies, radius=" <<ini_radius<< std::endl;
	std::vector<std::vector<double>> positions=direct_sphere(3,numbodies);
	std::vector<Particle*> product;
	
	for (std::vector<std::vector<double>>::iterator it = positions.begin() ; it != positions.end(); ++it) {
        const double x     = (*it)[0] * ini_radius;
        const double y     = (*it)[1] * ini_radius ;
		const double z     = flat_flag==0 ? (*it)[2] * ini_radius :0;
        const double rnorm = std::sqrt(sqr(x)+sqr(y)+sqr(z));
		const double v     = 2*M_PI/std::sqrt(rnorm*rnorm*rnorm);
        const double vx    = -y *v;
        const double vy    =  x *v;
		const double vz    = flat_flag==0 ? (std::rand()%2==0 ? 0.1*vx : -0.1*vx) : 0;
        product.push_back( new Particle( x, y, z, vx, vy,vz, mass) );
    }
	std::cout<<__FILE__ <<", " <<__LINE__<< ": Initialized " << numbodies << " bodies"<<std::endl;
	return product;
 }
 
 
/**
 *  Process command line options. Returns `true` iff execution is to continue.
 */
bool extract_options(int argc, char **argv) {
	int option_index = 0;
	int c;
	
	while ((c = getopt_long (argc, argv, "c:d:e:G:hi:m:n:p:r:Ss:t:v:",long_options, &option_index)) != -1)
    switch (c){
		case 'c':{
			std::stringstream param(optarg);
			param>>config_file_name;
			std::cout<<"Configuration File:="<<config_file_name<<std::endl;
			break;
		}
		
		case 'd':{
			std::stringstream param(optarg);
			param>>dt;
			std::cout<<"dt="<<dt<<std::endl;
			break;
		}
		
		case 'e':{
			std::stringstream param(optarg);
			param>>check_energy;
			std::cout<<"check_energy="<<check_energy <<std::endl;
			break;
		}
		
		case 'G':{
			std::stringstream param(optarg);
			param>>G;
			std::cout<<"G="<<G<<std::endl;
			break;
		}
		
		case 'h':{
			help( );
			return false;
		}
		
		case 'i':{
			std::stringstream param(optarg);
			param>>img_iter;
			std::cout<<"Frequency at which PNG images are written="<<img_iter<<std::endl;
			break;
		}
		
		case 'm':{
			std::stringstream param(optarg);
			param>>max_iter;
			std::cout<<"Number of iterations="<<max_iter<<std::endl;
			break;
		}
		
		case 'n':{
			std::stringstream param(optarg);
			param>>numbodies;
			std::cout<<"Number of bodies="<<numbodies<<std::endl;
			break;
		}
		
		case 'p':{
			std::stringstream param(optarg);
			param>>path;
			std::cout<<"Path="<<path<<std::endl;
			break;
		}
		
		case 'r':{
			std::stringstream param(optarg);
			param>>ini_radius;
			std::cout<<"Initial radius="<<ini_radius<<std::endl;
			break;
		}
		
		case 'S':{
			std::cout<<"Seed random number generator"<<std::endl;
			std::srand(1);
			break;
		}
		
		case 's':{
			std::stringstream param(optarg);
			param>>mass;
			std::cout<<"mass="<<mass<<std::endl;
			break;
		}
			
		case 't':{
			std::stringstream param(optarg);
			param>>theta;
			std::cout<<"Theta="<<theta<<std::endl;
			break;
		}
		
		case 'v':{
			std::stringstream param(optarg);
			param>>inivel;
			std::cout<<"Velocity="<<inivel<<std::endl;
			break;
		}
	}
	if (!ends_with(path,"/"))
		path.append("/");

	return true;
}	



/**
  * Generate help text
  */
void help() {
	std::cout << "Galaxy Simulator based on Barnes Hut code from University of Geneva." << std::endl<<std::endl;
	std::cout << "Parameters, showing default values" <<std::endl;
	std::cout << "\t-c,--config\t\tConfiguration file [" << config_file_name<<"]"<< std::endl;
	std::cout << "\t-d,--dt\t\tTime Step for Integration [" << dt<<"]"<< std::endl;
	std::cout << "\t-e,--check_energy\tCheck total energy every `check_energy` iterations[don't check]"<< std::endl;
	std::cout << "\t--flat\t\tUsed to set z to origin for 3D only"<< std::endl;
	std::cout << "\t-G,--G\t\tGravitational Constant [" << G << "]"<<std::endl;
	std::cout << "\t-h,--help\tShow help text" << std::endl;
	std::cout << "\t-i,--img_iter\tFrequency for writing positions [" << img_iter << "]"<< std::endl;
	std::cout << "\t-m,--max_iter\tMaximum number of iterations [" << max_iter << "]"<< std::endl;
	std::cout << "\t-n,--numbodies\tNumber of bodies [" << numbodies<< "]"<<std::endl;
	std::cout << "\t-p,--path\tPath for writing configurations [" << path << "]"<< std::endl;
	std::cout << "\t-r,--ini_radius\tInitial Radius [" << ini_radius << "]"<<std::endl;
	std::cout << "\t--resume\tResume previous run"<<std::endl;
	std::cout << "\t-s,--mass\tMass of bodies [" << mass << "]"<<std::endl;
	std::cout << "\t-t,--theta\tTheta-criterion of the Barnes-Hut algorithm [" << theta << "]"<< std::endl;
	std::cout << "\t-v,--inivel\tInitial velocities [" << inivel << "]"<<std::endl;
}

// --------------------------------------Old code that might be useful one day ------------------------------------------------

	// run_verlet(&get_acceleration_shm, max_iter, dt,	particles,	&print_values);
	// for (std::vector<Particle*>::iterator it = particles.begin() ; it != particles.end(); ++it) 
		// delete (*it);
	
	// std::vector<Body*> bodies; 
	// int iter=0;
    // if (resume_flag && restore_config(path,config_file_name, bodies,  iter,  theta,  G,  dt)) {
		// std::cout <<"Resume at "<<iter <<  ", theta="<<theta<<", G="<< G <<", dt="<<  dt << ", size="<< bodies.size() << std::endl;
		// simulate(iter, max_iter, bodies,  theta,  G,  dt,  img_iter, path,config_file_name,check_energy);
	// } else {
		// if (resume_flag)
			// std::cout << "Configuration file not found: starting from a new configuration" << std::endl;
		// else
			// std::cout << "Starting from a new configuration" << std::endl;
		// remove_old_configs(path);
		// bodies=createBodies(numbodies, inivel, ini_radius, mass );
		// simulate(0, max_iter, bodies,  theta,  G,  dt,  img_iter, path,config_file_name,check_energy);
	// }

  /**
  * Execute simulation
  */
 // void simulate(int start_iter,int max_iter,std::vector<Body*> bodies, double theta, double G, double dt, int img_iter,std::string path,std::string config_file_name,int check_energy) {
	// bool exiting=false;
	
	// double initial_energy=check_energy>0?get_kinetic_energy(bodies) + get_potential_energy(bodies,G) : -1;
	// double total_energy=initial_energy;
    // for (int iter=start_iter; iter<max_iter+start_iter && !exiting; ++iter) {
        // Node* root = NULL;    // The oct-tree is recomputed at each iteration.
        // for (unsigned i=0; i<bodies.size(); ++i) {
            // bodies[i] -> resetToZerothOctant();
            // root = add(bodies[i], root);
        // }
 
        // verlet(bodies, root, theta, G, dt); // Compute forces and advance bodies.
 
        // delete root;  // De-allocate the oct-tree.
		
		// exiting=killed();
        // if (iter%img_iter==0||exiting) {
            // std::cout << "Writing images at iteration " << iter << std::endl;
            // save_bodies(bodies, iter/img_iter,path);
			// save_config(bodies, iter, theta, G, dt,path,config_file_name);
        // }
		// if (check_energy>0 && iter%check_energy==0){
			// double energy=get_kinetic_energy(bodies) + get_potential_energy(bodies,G);
			// print_energy(energy, total_energy,initial_energy);
			// total_energy=energy;
		// }
    // }
	// if (check_energy>0 ){
		// double energy=get_kinetic_energy(bodies) + get_potential_energy(bodies,G);
		// print_energy(energy, total_energy,initial_energy);
		// total_energy=energy;
	// }
// }







 /**
  * Restore configuration from saved file
  */
// bool restore_config(std::string path,std::string name,std::vector<Body*>& bodies, int& iter, double &theta, double &G, double &dt) {
	// std::stringstream file_name;
    // file_name << path<< name;
	// std::ifstream config_file(file_name.str().c_str());

    // if(! config_file.is_open())
        // return false;
	// enum State{expect_version, expect_iteration, expect_theta, expect_g, expect_dt, expect_body, expect_eof};
	// State state=State::expect_version;
	// while(! config_file.eof())   {
		// std::string line;
        // getline(config_file,line);
        // std::stringstream ss(line);
		// std::string token;
		// switch(state) {
			// case State::expect_version:
				// token = line.substr(1+line.find("="));
				// std::cout << token << std::endl;
				// state=State::expect_iteration;
				// break;
			// case State::expect_iteration:
				// token = line.substr(1+line.find("="));
				// iter=atoi(token.c_str());
				// std::cout << iter << std::endl;
				// state=State::expect_theta;
				// break;
			// case State::expect_theta:
				// token = line.substr(1+line.find("="));
				// std::cout << decode(token) << std::endl;
				// theta=decode(token);
				// state=State::expect_g;
				// break;
			// case State::expect_g:
				// token = line.substr(1+line.find("="));
				// std::cout << decode(token) << std::endl;
				// G=decode(token);
				// state=State::expect_dt;
				// break;
			// case State::expect_dt:
				// token = line.substr(1+line.find("="));
				// std::cout << decode(token) << std::endl;
				// dt=decode(token);
				// state=State::expect_body;
				// break;
			// case State::expect_body:
				// if (line.find("End")==0)
					// state=State::expect_eof;
				// else
					// bodies.push_back(extract_body(line));
				// break;
			// case State::expect_eof:
				// if (line.length()>0){
					// std::cout<<"Unexpected text following end"<<std::endl;
					// std::cout<<line<<std::endl;
					// return false;
				// }
			// default:
				// if (line.length()>0){
					// std::cout<<"Unexpected state: "<<state<<std::endl;
					// return false;
				// }
		// }
    // }
	// if (state!=State::expect_eof) {
		// std::cout<<"Unexpected state: "<<state<<"-" <<State::expect_eof <<std::endl;
		// return false;
	// }
	// return true;
// }

/**
 * Retrieve position, mass, and velocities stored for one Body
 */
// Body * extract_body(std::string line){
	// enum State {expect_i,expect_x,expect_y,expect_z,expect_m,expect_vx,expect_vy,expect_vz,end_line};
	// State state=expect_i;
	// double px, py, pz, m, vx,vy,vz;
	// while (state!=end_line) {
		// int pos=line.find(",");
		// std::string token=pos>=0 ? line.substr(0,pos) : line;
		// line=line.substr(pos+1);
	
		// switch (state){
			// case expect_i:
				// state=expect_x;
				// break;
			// case expect_x:
				// state=expect_y;
				// px=decode(token);
				// break;
			// case expect_y:
				// state=expect_z;
				// py=decode(token);
				// break;
			// case expect_z:
				// state=expect_m;
				// pz=decode(token);
				// break;
			// case expect_m:
				// state=expect_vx;
				// m=decode(token);
				// break;
			// case expect_vx:
				// vx=decode(token);
				// state=expect_vy;
				// break;
			// case expect_vy:
				// vy=decode(token);
				// state=expect_vz;
				// break;
			// case expect_vz:
				// vz=decode(token);
				// state=end_line;
		// }
	// }
	// return new Body(m,px,py,pz,vx,vy,vz);
// }


// void save_config( std::vector<Body*>& bodies, int iter, double theta, double G, double dt, std::string path,std::string name) {
	// std::stringstream file_name;
    // file_name << path<< name;
// /**
 // * Save configuration so it can be restarted later
 // */	backup(file_name.str().c_str());
    // std::ofstream ofile(file_name.str().c_str());
	// ofile << "Version="<<config_version<<"\n";
	// ofile << "iteration=" << iter  << "\n";
	// ofile << "theta=" << encode(theta)  << "\n";
	// ofile << "G=" << encode(G)  << "\n";
	// ofile << "dt=" << encode(dt)  << "\n";
    // for (unsigned i=0; i<bodies.size(); ++i) {
		// double px, py, pz;
		// bodies[i] -> getPos(px, py,pz);
		// double vx, vy,vz;
		// bodies[i] -> getVel(vx, vy,vz);
		// double m=bodies[i]->getMass();
		// ofile <<i<<","<< encode(px)<<","<< encode(py)<<","<< encode(pz)<<","<< encode(m) <<","<< encode(vx)<<","<< encode(vy)<<","<< encode(vz)<<"\n";
	// }
	// ofile << "End\n";
// }



 
 

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
 */
 
 #include "particle.hpp"
 
 using namespace std;
 
/**
 *   Create one stationary particle at origin with unit mass
 */
 Particle::Particle() :_id(-1),_m(1.0) {
	for (int i=0;i<DIM;i++){
		_position[i] = 0.0;
		_velocity[i] = 0.0;
		_acceleration[i] = 0.0;
	}
}

/**
 *   Used to set initial position and velocity when configuration is initialized.
 */
 void Particle::init(array<double,DIM> position, array<double,DIM> velocity, double m, int id) {
	_id = id;
	_m = m;
	_position = position;
	_velocity = velocity;
	_acceleration = {0.0, 0.0, 0.0};
 }
 

 
 /**
 * Output position, velocity, and mass.
 */
ostream& operator<<(ostream& s, Particle& p) {
	return s<< p._id <<","<<p._position[0] <<"," << p._position[1] <<"," << p._position[2] <<"," <<
			p._velocity[0] <<"," << p._velocity[1] <<"," << p._velocity[2] <<"," <<p._m;
}
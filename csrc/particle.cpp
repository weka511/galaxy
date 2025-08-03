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
 
 void Particle::init(array<double,3> position, array<double,3> velocity, double m, int id) {
	 _id = id;
	 _m = m;
	for (int i=0;i<3;i++) {
		_position[i] = position[i];
		_velocity[i] = velocity[i];
		_acceleration[i] = 0.0;
	}	 
 };
 

 
 /**
 * Output position, velocity, and mass.
 */
ostream& operator<<(ostream& s, Particle& p) {
	return s<< p._id <<","<<p._position[0] <<"," << p._position[1] <<"," << p._position[2] <<"," <<
			p._velocity[0] <<"," << p._velocity[1] <<"," << p._velocity[2] <<"," <<p._m;
}
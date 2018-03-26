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
 
 #include "particle.h"
 
 /**
 * Output position, velocity, and mass.
 */
std::ostream& operator<<(std::ostream& s, Particle& p) {
	return s<< p._x <<"," << p._y <<"," << p._z <<"," <<
			p._vx <<"," << p._vy <<"," << p._vz <<"," <<p._m;
}
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
 
#ifndef _PHYSICS_H
#define _PHYSICS_H

#include <vector>
#include "particle.h"

const double seconds_in_one_year = 365.25*24*60*60;

const double mass_earth = 5.9722e24;  // https://en.wikipedia.org/wiki/Earth_mass

const double mass_sun = 1.98855e30;  // https://en.wikipedia.org/wiki/Solar_mass

const double metres_per_AU = 149597870700; // https://en.wikipedia.org/wiki/Astronomical_unit

const double au_per_m = 1/metres_per_AU;
const double G_SI = 6.67408e-11; // https://en.wikipedia.org/wiki/Gravitational_constant

const double G_solar_system = G_SI * au_per_m*au_per_m*au_per_m *(mass_sun)*(seconds_in_one_year *seconds_in_one_year);

void get_acceleration(std::vector<Particle*> particles,double G);

void get_acceleration_between_pair(Particle* p_i,Particle* p_j,double G);

#endif

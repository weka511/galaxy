/**
 * Copyright (C) 2019-2025 Greenweaves Software Limited
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
 * This file exercises serialization. 
 */
 
 #include "catch.hpp"
 #include "configuration.hpp"

class TestConfiguration: public Configuration::Visitor,  public Configuration::PairVisitor{
	virtual void visit(Particle & particle){;}
	virtual void visit_pair(Particle & particle1,Particle & particle2){;}
};
	

 TEST_CASE( "Configuration Tests", "[configuration]" ) {
	 
	 SECTION("...") {  
	

	}
 }
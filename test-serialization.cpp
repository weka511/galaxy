/**
 * Copyright (C) 2019 Greenweaves Software Limited
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
 #include "utils.h"


 TEST_CASE( "Serialization Tests", "[serialization]" ) {
	 
	 SECTION("Parse float") {
		 REQUIRE(deserialize("3ffa08e64ff5b9ea")==1.6271727679664942);
		REQUIRE(deserialize("4008d3ce555810e5")==3.103420893430768);
		REQUIRE(deserialize("40136dfdeeec7dbe")==4.857413991150166);
		REQUIRE(deserialize("3fa548b00dcb0730")==0.0415701882207159);
		REQUIRE(deserialize("40113b1a3e4eb0da")==4.307717298081309);
		REQUIRE(deserialize("3ff234f95772daca")==1.1379331031763962);
		REQUIRE(deserialize("3fe40c475a2f89d5")==0.6264988671721833);
		REQUIRE(deserialize("3fff9a7b373b0ee5")==1.975215163941294);
		REQUIRE(deserialize("3ffc1bcc1bb8ac1a")==1.7567864497057202);
		REQUIRE(deserialize("3ff51789c227032b")==1.3182466110303703);

		// double dd = deserialize("400921fb54442d18");
		
		// REQUIRE(dd==3.141592653589793);
		// REQUIRE(deserialize("x3ff2055897270e30")==-3.74738962089598);
		// REQUIRE(deserialize("x3fed5a54ed4d24ef")==-4.661785404364097);
		// REQUIRE(deserialize("x3ffa04ad0cfc82cc")==-2.747716806930521);
		// REQUIRE(deserialize("x3fee7d370d3a22f0")==-4.377719682051307);
		// REQUIRE(deserialize("4011f0b4f13fca8a")==4.485065240400795);
		// REQUIRE(deserialize("401d17e370ce8898")==-0.5908339306586017); //x
		// REQUIRE(deserialize("4000c051e1c8611c")==2.0939061774516983);
		// REQUIRE(deserialize("3ff69d8eb1250114")==1.4134661597927947);
		// REQUIRE(deserialize("x3fee3fb257a9c30f")==-4.437796240104249);
		// REQUIRE(deserialize("4013d06d4fcb0f14")==4.953541991013271);

	}
 }
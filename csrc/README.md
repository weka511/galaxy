# galaxy

Implementation of the [Barnes Hut](https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation) algorithm
to simulate the evolution of a galaxy. This code has been compiled under [Cygwin](https://www.cygwin.com/).



| File | Header | Purpose |
|---------------------|------------------|---------------------------------------------------------------------|
| barnes-hut.cpp |barnes-hut.h| Calculate accelerations using Barnes Hut|
| |catch.hpp | [Catch2](https://github.com/catchorg/Catch2) Unit testing framework |
| center-of-mass.cpp |center-of-mass.h| Find centre of mass for internal nodes|
| collision.xml||Parameter file to set up for colliding galaxies |
| configs.cpp | configs.h| Configuration management |
| factory.cpp | factory.h| Abstract Factory for creating configurations of particles |
| galaxy.cpp |galaxy.h| Simulate Galaxy using Barnes Hut|
| GalaxySimulator.xml||Notepad++ Workspace|
| Makefile || Build galaxy simulation |
| mt.cc |mt.h| Mersenne twister|
| mtex.cc || Test code for Mersenne twister |
| mttest.cc || Test code for Mersenne twister  |
| particle-factory.cpp | particle-factory.h | Create particles as specified by an XML file |
| particle.h| particle.h | Represents the particles whose motion is being simulated|
| physics.cpp| physics.h |Physical constants, law of gravitation, etc|
| plummer.cpp | plummer.h |Factory for creating particles |
| |spdlog| Header files for [spdlog](https://github.com/gabime/spdlog) super fast C++ logging library.|
| tinyxml2.cpp | tinyxml2.h |[Lee Thomason's XML Parser](http://www.grinninglizard.com/tinyxml/)|
| stepper.cpp| stepper.h | A class to support multi -threading |
| tests.cpp || Test harness |
| test-kepler.cpp| | Test for Kepler problem |
| test-plummer.cpp| | Test for Plummer model |
| test-random.cpp| | Test for Various random number generators |
| test-serialization.cpp| | Test that serialization works OK |
| test-tree.cpp| | Test treecode |
| threading.cpp || Test to determine whether threads can speed up code |
| timing.sh || Script to determine execution times |
| treecode.cpp | treecode.h | Barnes-Hut Tree|
| utils.cpp | utils.h | Assorted utility functions|
| verlet.cpp | verlet.h | [Verlet "leapfrog"](http://physics.ucsc.edu/~peter/242/leapfrog.pdf) integrator|
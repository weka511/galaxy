# galaxy

Implementation of the [Barnes Hut](https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation) algorithm
to simulate the evolution of a galaxy. This code has been compiled under [Cygwin](https://www.cygwin.com/).



 File | Header | Purpose 
---------------------|------------------|---------------------------------------------------------------------
 acceleration.cpp |  acceleration.hpp |Calculates the acceleration for each particle 
 barnes-hut.cpp | barnes-hut.hpp | Used the Oct-tree to drive acceleration.cpp
catch.hpp || [Catch2](https://github.com/catchorg/Catch2) Unit testing framework 
 center-of-mass.cpp | center-of-mass.hpp |Calculate centre of mass for Internal and External Nodes 
 configuration.cpp | configuration.hpp| Manages the collection of Particlest 
 galaxy.cpp |galaxy.hpp| Main program; parses command line parameters and initializes other classes
 Makefile || Build galaxy simulation 
 particle.cpp| particle.hpp | Represents the particles whose motion is being simulated
 reporter.cpp |  reporter.hpp|Record the configuration periodically 
 treecode.cpp |treecode.hpp  | The Barnes Hut Oct-tree
 verlet.cpp | verlet.hpp | Integrate an Ordinary Differential Equation using the Verlet algorithm
 tests.cpp || Test harness 
 test-configuration.cpp| | Test that serialization works OK 
 test-particle.cpp |  | 
 test_treecode.cpp |  | 

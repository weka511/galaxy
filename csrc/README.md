# galaxy

Implementation of the [Barnes Hut](https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation) algorithm
to simulate the evolution of a galaxy. This code has been compiled under [Cygwin](https://www.cygwin.com/).

## Code

 File|Header|Purpose 
---------------------|------------------|---------------------------------------------------------------------
acceleration.cpp|acceleration.hpp|Calculates the acceleration for each particle 
barnes-hut.cpp|barnes-hut.hpp|Used the Oct-tree to drive acceleration.cpp
-|catch.hpp|[Catch2]( https://github.com/catchorg/Catch2/tree/v2.x/single_include/catch2) Unit testing framework 
center-of-mass.cpp|center-of-mass.hpp|Calculate centre of mass for Internal and External Nodes 
configuration.cpp|configuration.hpp|Manages the collection of Particlest 
galaxy.cpp||Main program; parses command line parameters and initializes other classes
integrators.cpp|integrators.hpp|Integrate an Ordinary Differential Equation using the Leapfrog algorithm
logger.cpp|logger.hpp|Record messages in logfile
Makefile||Build galaxy simulation 
parameters.cpp|parameters.hpp|Command line parameters and environment variables.
particle.cpp|particle.hpp|Represents the particles whose motion is being simulated
reporter.cpp|reporter.hpp|Record the configuration periodically 
tests.cpp||main() for unit tests 
test-configuration.cpp||Test that serialization works OK
test-integrators.cpp||Tests for integrators.cpp 
test-particle.cpp||Tests for particle.cpp 
test_treecode.cpp||Tests for treecode.cpp
treecode.cpp|treecode.hpp|The Barnes Hut Oct-tree
tree-verifier.cpp|tree-verifier.hpp|Used to test a tree build by treecode

# galaxy

Implementation of the [Barnes Hut](https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation) algorithm
to simulate the evolution of a galaxy. This code has been compiled under [Cygwin](https://www.cygwin.com/).



| File | Header | Purpose |
|---------------------|------------------|---------------------------------------------------------------------|
| |catch.hpp | [Catch2](https://github.com/catchorg/Catch2) Unit testing framework |
| configuration.cpp | configs.hpp| Configuration management |
| galaxy.cpp |galaxy.hpp| Simulate Galaxy using Barnes Hut|
| Makefile || Build galaxy simulation |
| particle.cpp| particle.hpp | Represents the particles whose motion is being simulated|
| tinyxml2.cpp | tinyxml2.h |[Lee Thomason's XML Parser](http://www.grinninglizard.com/tinyxml/)|
| tests.cpp || Test harness |
| test-serialization.cpp| | Test that serialization works OK |
| utils.cpp | utils.hpp | Assorted utility functions|
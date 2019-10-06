# galaxy

Implementation of the [Barnes Hut](https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation) algorithm
to simulate the evolution of a galaxy. This code has been compiled under [Cygwin](https://www.cygwin.com/).

## Acknowledgements

I learned about the Barnes Hut Algorithm from the [Simulation and modeling of natural processes](https://www.coursera.org/learn/modeling-simulation-natural-processes/home/info) from the University of Geneva.

I am grateful to the following people who have geneously made code available the the world at large:

* Jason Blevins for [his version](https://jblevins.org/projects/mt) of the [Mersenne Twister by Mutsuo Saito and Makoto Matsumoto](http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html);
* Martin Hořeňovský for the [_Catch2_ Unit testing framework](https://github.com/catchorg/Catch2);
* Gabi Melman for the [_spdlog_ Fast C++ Logging Library](https://github.com/gabime/spdlog);
* Lee Thomason for the [_TinyXML2_ XML Parser](http://www.grinninglizard.com/tinyxml/).

*NB* Their source code is available under whichever licences the authors have used. It is _not_ covered by the license for the _galaxy_ package.

## Contents

### galaxy.exe

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

### Helper programs

| File |  Purpose |
|---------------------|---------------------------------------------------------------------------------------|
| adze.py |Cut logfiles up and analyze data |
| configure.py | Create configuration for galaxy.exe |
| make_3d.py | Plot output from galaxy |
| make_img.py | Plot output from galaxy |
| plot_energy.py  |Used to investigate distribution of energies - do we thermalize?| 
| plot_orbits.py | Select a few stars at random and plot their orbits |

## <a name="notes"> Implementation Notes

|  Date | Remarks |
|------------|--------------------------------------------------------------------|
|31&nbsp;Jan&nbsp;18| Program segfaults. Turn off Barnes-Hut and try some easy test cases, e.g. Kepler, Lagrange points, and a resonance |
| 4 Feb 18| Traced problem to acceleration calcs: when we figure out force on a particular particle we included the particle itself. This has been corrected, but [the fix is a kludge, so some work still needs to be done](https://github.com/weka511/galaxy/issues/2).<br>_Particles are diverging,_ is the initial velocity too high?|
| 6 Feb 18 | Reset centre of mass and linear momentum to (0,0,0) at start of run, and checked code. Configuration still d rifts. Need to construct simplest possible case that exhibits error. | 
| 7 Feb 18 | I have done some more checking by adding tracing statements, and code appears to be working, albeit with large error. I am studying [ Error analysis of a tree code by Barnes and Hut.](http://adsabs.harvard.edu/full/1989ApJS...70..389B) Do errors decrease for very many particles? What if we soften gravitational potential? Another thought: what is error in Geneva code?|
| 8 Feb 18 | Downloaded Geneva version and modified to display energy. At present this does not appear to be conserved |
|13&nbsp;Feb&nbsp;18| I am logging parameters, so can now investigate dependence of energy anomaly on dt, softening, and theta. I'm also doing some long runs to study long-term behaviour.|
|26 Feb 18| The kinetic energy and potential energy aren't following Virial theorem very well. I will check mass distribution first.|
|17&nbsp;Mar&nbsp;18| After a week of development I now have a multithreaded version. I don't think I'd have started if I'd realized how much work it was. Just doing a few time comparisons. Later: very disappointing - multi-threaded is slower than single threaded.| 
|18 Mar 18|What if there many more than 1000 bodies? We possibly need the acceleration calculation to be much longer to see the benefit of threads? No: I have just tested, with 100000 bodies, 10 iterations. 4 threads, 448 seconds; 2 threads, 448; unthreaded, 165|
|&nbsp;| I have created a factory to allow more complex simulations, e.g. colliding galaxies. I decided to use XML rather than CSV, and used [Lee Thomason's TinyXML-2](http://leethomason.github.io/tinyxml2/index.html), following the discussion [here](https://stackoverflow.com/questions/170686/what-is-the-best-open-xml-parser-for-c). XML is more than I require at present, but I expect the complexity of the input files weill increase, so this is by way of future proofing.|
|25 Mar 18|I'm curious to see whether the configuration eventually becomes thermalized--see [Debarshee Bagchi and Constantino Tsallis: Fermi-Pasta-Ulam-Tsingou problems: Passage from Boltzmann to qq-statistics](https://arxiv.org/abs/1709.08729). Since we were throwing away the momentum when we write the configuration, I've raised [Issue #32](https://github.com/weka511/galaxy/issues/32) to fix this. This is also a good time to simplify the code by removing energy calcs, so we use c++ for heavy lifting, but do as much as possible in Python.|
| 26 Mar 18|This shows the distribution after 5400 iterations, compared with the Bolzmann Distribution. ![Alt text](energy05400.png?raw=true "Title") Here is version from earlier in the simulation, which is not very Boltzmann.![Alt text](energy00200.png?raw=true "Title")This shows the evolution of the beta parameter. ![Alt text](boltzmann.png?raw=true "Title") |
| 2 Apr 18 | Revisit Virial Theorem - what is the effect of the softening length? |

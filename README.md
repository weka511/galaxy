# galaxy

Implementation of the [Barnes Hut](https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation) algorithm
to simulate the evolution of a galaxy. This code has been compiled under [Cygwin](https://www.cygwin.com/), and it currently under test.

## Disclaimer

This code is under test, and should not be relied on. See [Implementation Notes](#notes)

## Contents

| File | Header | Purpose |
|---------------------|------------------|---------------------------------------------------------------------|
| barnes-hut.cpp |barnes-hut.h| Calculate accelerations using Barnes Hut|
| |catch.hpp | [Catch2](https://github.com/catchorg/Catch2) Unit testing framework |
| center-of-mass.cpp |center-of-mass.h| Find centre of mass for internal nodes|
| galaxy.cpp |galaxy.h| Simulate Galaxy using Barnes Hut|
| geneva.cpp | |2D Barnes Hut code from University of Geneva, modified to calculate total energy |
|Galaxy Simulator.xml||Notepad++ Workspace|
| Makefile || Build Barnes Hut simulation |
| make_3d.py | |Plot output from galaxy |
| make_img.py || Plot output from galaxy |
|| particles.h | Represents the particles whose motion is being simulated|
|physics.cpp| physics.h |Physical constants, law of gravitation, etc|
| plot_orbits.py || Select a few stars at random and plot their orbits |
| tests.cpp || Test harness |
| test-kepler.cpp| | Test for Kepler problem |
| treecode.cpp | treecode.h | Barnes-Hut Tree|
| utils.cpp | utils.h | Assorted utility functions|
| verlet.cpp | verlet.h | [Verlet "leapfrog"](http://physics.ucsc.edu/~peter/242/leapfrog.pdf) integrator|

## <a name="notes"> Implementation Notes

|  Date | Remarks |
|------------|--------------------------------------------------------------------|
|31_Jan_2018| Program segfaults. Turn off Barnes-Hut and try some easy test cases, e.g. Kepler, Lagrange points, and a resonance |
| 4_Feb_2018| Traced problem to acceleration calcs: when we figure out force on a particular particle we included the particle itself. This has been corrected, but [the fix is a kludge, so some work still needs to be done](https://github.com/weka511/galaxy/issues/2).<br>_Particles are diverging,_ is the initial velocity too high?|
| 6_Feb_2018 | Reset centre of mass and linear momentum to (0,0,0) at start of run, and checked code. Configuration still drifts. Need to construct simplest possible case that exhibits error. | 
| 7_Feb_2018 | I have done some more checking by adding tracing statements, and code appears to be working, albeit with large error. I am studying [ Error analysis of a tree code by Barnes and Hut.](http://adsabs.harvard.edu/full/1989ApJS...70..389B) Do errors decrease for very many particles? What if we soften gravitational potential? Another thought: what is error in Geneva code?|
| 8_Feb_2018 | Downloaded Geneva version and modified to display energy. At present this does not appear to be conserved |

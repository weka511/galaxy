# galaxy

Implementation of the [Barnes Hut](https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation) algorithm
to simulate the evolution of a galaxy.

## Disclaimer: this code is under test, and should not be relied on.

## Contents

| File | Header | Purpose |
|---------------------|------------------|---------------------------------------------------------------------|
| barnes-hut.cpp |barnes-hut.h| Calculate accelerations using Barnes Hut|
| |catch.hpp | [Catch2](https://github.com/catchorg/Catch2) Unit testing framework |
| center-of-mass.cpp |center-of-mass.h| Find centre of mass for internal nodes|
| galaxy.cpp |galaxy.h| Simulate Galaxy using Barnes Hut|
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

## Implementation Notes

|  Date | Remarks |
|------------|--------------------------------------------------------------------|
|31_Jan_2018| Program segfaults. Turn off Barnes-Hut and try some easy test cases, e.g. Kepler, Lagrange points, and a resonance |
| 4_Feb_2018| Traced problem to acceleration calcs: when we figure out force on a particular particle we included the particle itself. This has been corrected, but [the fix is a kludge, so some work still needs to be done](https://github.com/weka511/galaxy/issues/2).<br>_Particles are diverging,_ is the initial velocty too high?|

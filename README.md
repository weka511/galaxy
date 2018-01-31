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
|| physics.h |Physical constants|
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

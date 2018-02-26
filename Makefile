# Copyright (C) 2018 Greenweaves Software Limited
#
# This is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software.  If not, see <http://www.gnu.org/licenses/>
#
# Makefile snarfed from https://stackoverflow.com/questions/2481269/how-to-make-a-simple-c-makefile

CPPFLAGS=-g -O3  -D_RUNTIME_CHECKS -pthread -I/sw/include/root  -std=gnu++11
LDFLAGS=-g -O3
LDLIBS=
CC=gcc
CXX=g++
RM=rm -f
MKDIR=mkdir
SRCS=	barnes-hut.cpp		\
		center-of-mass.cpp	\
		configs.cpp			\
		mt.cc				\
		physics.cpp         \
		plummer.cpp			\
		treecode.cpp		\
		utils.cpp			\
		verlet.cpp

TESTS= 	test-kepler.cpp		\
		test-plummer.cpp	\
		test-tree.cpp
		
OBJS=$(subst .cpp,.o,$(SRCS)) 

TEST_OBJS=$(subst .cpp,.o,$(TESTS))

MAIN=galaxy.exe
TEST_MAIN=tests.exe

TARGETS=$(MAIN) 

all : $(TARGETS) $(TEST_OBJS)

run : all
	$(MAIN)
	
tests : $(TEST_MAIN)
	./$(TEST_MAIN)
	python make_3d.py kepler.csv
	python make_3d.py -n 3 lagrange.csv

mersenne : mtex.exe mttest.exe

mtex.o :  mtex.cc

mtex.exe : mt.o mtex.o
	${CXX} $(LDFLAGS) -o mtex.exe mt.o mtex.o ${LDLIBS}

mttest.exe : mttest.o mt.o
	${CXX} $(LDFLAGS) -o mttest.exe mt.o mttest.o ${LDLIBS}	
	
clean :
	${RM} *.o *.stackdump

rebuild: clean all

depend: .depend

.depend: $(SRCS)  galaxy.cpp $(TESTS) tests.cpp
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;
	sed -i -e 's/\/home\/Weka\/galaxy\///$g' .depend
	
$(MAIN): $(OBJS) galaxy.o mt.o
	${CXX} $(LDFLAGS) -o $(MAIN) galaxy.o ${OBJS} ${LDLIBS}
	
$(TEST_MAIN): $(OBJS) tests.o $(TEST_OBJS)
	${CXX} $(LDFLAGS) -o $(TEST_MAIN) tests.o ${OBJS} $(TEST_OBJS) ${LDLIBS}
	
distclean: clean
	$(RM) *~ .depend

setup:
	-$(MKDIR) configs
	-$(MKDIR) imgs
	-$(MKDIR) logs
	
include .depend

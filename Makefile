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

CPPFLAGS=-g -O3 -pthread -I/sw/include/root  -std=gnu++11
LDFLAGS=-g -O3
LDLIBS=
CC=gcc
CXX=g++
RM=rm -f
MKDIR=mkdir
SRCS=	barnes-hut.cpp		\
		center-of-mass.cpp	\
		treecode.cpp		\
		utils.cpp			\
		verlet.cpp
		
OBJS=$(subst .cpp,.o,$(SRCS))
MAIN=galaxy.exe

TARGETS=$(MAIN) 

all : $(TARGETS)

clean :
	${RM} *.o *.stackdump
	
depend: .depend

.depend: $(SRCS)  galaxy.cpp 
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;
	
$(MAIN): $(OBJS) galaxy.o
	${CXX} $(LDFLAGS) -o $(MAIN) galaxy.o ${OBJS} ${LDLIBS}
	

distclean: clean
	$(RM) *~ .depend

setup:
	-$(MKDIR) configs
	-$(MKDIR) imgs
	
include .depend

# Creation of the exe file
# WARNING : this Makefile is made to run only on a UNIX System
#
# 14/11/2017 - clement.decoopman@univ.etu-amu.fr


#!/bin/bash

# Definition of particular target
# only if files named the same way exist
.PHONY: clean, distclean


# Disable implicit rules
#.SUFFIXES:


# Variables for Unix/X11
SHELL    = /bin/bash

# REPLACE CPP by "C" to get gcc
# Remove CXXFLAGS if running in C

VERSION  = CPP
ifeq ($(VERSION),CPP)
CXX = g++
else
CXX = gcc
endif

CFLAGS 	 = -g -Wall -v -ansi -pedantic -O2
CXXFLAGS = $(CFLAGS)	-std=c++11
#LIBS     = -L/usr/X11R6/lib -lX11 -lXext
THREAD   = -lpthread
RM			 = rm -f

# Put your C++ exe after EXECPP =
EXECPP 	 = prog



all :: $(EXECPP)

%.o: %.c
	$(CXX) -c $(CXXFLAGS) $*.cpp


prog : integrale_thread.o
	$(CXX) -o $@ $^ $(THREAD)

depends :
	$(CXX) -MM $(CXXFLAGS) *.cpp >depends

clean ::
	$(RM) *.o

distclean ::
	$(RM) *% *~ .*.swp $(EXECPP)

include depends

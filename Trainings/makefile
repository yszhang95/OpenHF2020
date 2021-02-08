# Original code can be found on
# https://github.com/root-project/root/blob/v6-06-00-patches/tutorials/tmva/makefile
# Modified by Yousen Zhang, Rice University
# Date: 2020 August 27

######################################################################
# Project: TMVA - Toolkit for Multivariate Data Analysis             #
# Code   : Example usage for TMVA analyses                           #
###################################################################### 

CXX = g++
CCFLAGS = -O0 -ggdb -std=c++11 


LD = g++
LDFLAGS = 

LIBS = $(shell root-config --libdir) 
INCS = $(shell root-config --incdir)  


default : bin/tmvaClassfication

#bin/tmvaClassfication : src/tmvaClassfication.C 
bin/tmvaClassfication : src/TMVAClassfication.cc
	@echo -n "Building $@ ... "
	$(CXX) $(CCFLAGS) $<  $(addprefix -I, $(INCS))  $(shell root-config --libs) -lMLP -lMinuit -lTreePlayer -lTMVA -lTMVAGui -lXMLIO  -lMLP -lm  -g -o $@
	@echo "Done"


clean: 
	rm -f *.o	$(BINS)

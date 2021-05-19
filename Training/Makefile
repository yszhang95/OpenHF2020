# Original code can be found on
# https://github.com/root-project/root/blob/v6-06-00-patches/tutorials/tmva/makefile
# Modified by Yousen Zhang, Rice University
# Date: 2020 August 27

######################################################################
# Project: TMVA - Toolkit for Multivariate Data Analysis             #
# Code   : Example usage for TMVA analyses                           #
###################################################################### 

CXX = g++
CCFLAGS = -O0 -ggdb
CCFLAGS += $(shell root-config --auxcflags)


LD = g++
LDFLAGS = 

LIBS = $(shell root-config --libdir) 
INCS = $(shell root-config --incdir)  


default : bin/TMVAClassification bin/TMVAClassificationApp

bin/TMVAClassificationApp: bin/TMVAClassificationApp.o bin/functions.o
	@echo -n "Linking $@ ... "
	$(CXX) $(CCFLAGS) $^ $(shell root-config --libs) -L$(OPENHF2020TOP)/Utilities/lib \
		-lMyTreeReader -lGenVector -lMLP -lMinuit -lTreePlayer -lTMVA -lTMVAGui -lXMLIO  -lMLP -lm  -g -o $@

bin/TMVAClassification: bin/TMVAClassification.o bin/functions.o
	@echo -n "Linking $@ ... "
	$(CXX) $(CCFLAGS) $^ $(shell root-config --libs) -L$(OPENHF2020TOP)/Utilities/lib \
		-lMyTreeReader -lGenVector -lMLP -lMinuit -lTreePlayer -lTMVA -lTMVAGui -lXMLIO  -lMLP -lm  -g -o $@

bin/%.o: src/%.cc
	@echo -n "Building $@ ... "
	$(CXX) $(CCFLAGS) $<  $(addprefix -I, $(INCS)) -Iinclude -I$(OPENHF2020TOP)/Utilities  -g -c -o $@
	@echo "Done"

clean: 
	rm -f *.o	$(BINS)

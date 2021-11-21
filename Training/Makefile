# Original code can be found on
# https://github.com/root-project/root/blob/v6-06-00-patches/tutorials/tmva/makefile
# Modified by Yousen Zhang, Rice University
# Date: 2020 August 27

######################################################################
# Project: TMVA - Toolkit for Multivariate Data Analysis             #
# Code   : Example usage for TMVA analyses                           #
######################################################################

CXX=$(shell root-config --cxx)
CCFLAGS = -O2
CCFLAGS += $(shell root-config --cflags)

LDFLAGS=$(shell root-confi --libs)


LIBS = $(shell root-config --libdir)
INCS = $(shell root-config --incdir)

BIN = bin bin/TMVACrossValidation bin/TMVACrossValidationApp bin/TMVAClassification bin/TMVAClassificationApp bin/TMVAClassificationAppNtuple

default : $(BIN)

bin/TMVA%: bin/TMVA%.o bin/functions.o
	@echo -n "Linking $@ ... "
	$(CXX) $(CCFLAGS) $^ $(shell root-config --libs) -L$(OPENHF2020TOP)/Utilities/lib \
		-lMyTreeReader -lGenVector -lMLP -lMinuit -lTreePlayer -lTMVA -lTMVAGui -lXMLIO \
		-lRooFit -lRooFitCore -lMLP -lm  -g -o $@

bin/%.o: src/%.cc
	@echo -n "Building $@ ... "
	$(CXX) $(CCFLAGS) $<  $(addprefix -I, $(INCS)) -Iinclude -I$(OPENHF2020TOP)/Utilities  -g -c -o $@
	@echo "Done"

bin:
	@echo "Creating directory bin"
	@mkdir bin
	@echo "Done"
	@echo

.PHONY: clean
clean:
	rm -r bin

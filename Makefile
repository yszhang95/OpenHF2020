OPENHF2020TOPDIR:=${OPENHF2020TOP}

CXX=$(shell root-config --cxx)

CXXFLAGS=$(shell root-config --cflags)
CXXFLAGS+=-I.
CXXFLAGS+=-I$(OPENHF2020TOPDIR)
CXXFLAGS+=-O2

CCFLAGS:=-O2
CCFLAGS+=$(shell root-config --cflags)

LDFLAGS:=$(shell root-config --ldflags)
LDFLAGS+=$(shell root-config --libs)

LDGLIBFLAGS:=$(shell root-config --ldflags)
LDGLIBFLAGS+=$(shell root-config --glibs)

ROOTLIBS:=$(shell root-config --libdir)
ROOTINCS:=$(shell root-config --incdir)



.PHONY: all
all::
	@echo Start building...
	@echo

-include Utilities/Makefile.inc
-include Training/Makefile.inc

all:: $(MYLIB) $(MYMVABINDIR) $(MYLIB)/libMyTreeReader.so $(MYLIB)/libMyFitUtils.so $(MYMVABINS)

.PHONY: clean
clean:
	rm -rf $(MYLIB) $(MYMVABINDIR)

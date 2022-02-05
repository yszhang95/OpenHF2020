# Original code can be found on
# https://github.com/root-project/root/blob/v6-06-00-patches/tutorials/tmva/makefile
# Modified by Yousen Zhang, Rice University
# Date: 2020 August 27

######################################################################
# Project: TMVA - Toolkit for Multivariate Data Analysis             #
# Code   : Example usage for TMVA analyses                           #
######################################################################

MYMVASRCDIR := $(OPENHF2020TOPDIR)/Training/src
MYMVABINDIR := $(OPENHF2020TOPDIR)/Training/bin
MYMVABINS := \
	$(MYMVABINDIR)/TMVACrossValidation \
	$(MYMVABINDIR)/TMVACrossValidationApp \
	$(MYMVABINDIR)/TMVAClassification \
	$(MYMVABINDIR)/TMVAClassificationApp \
	$(MYMVABINDIR)/TMVAClassificationAppNtuple


$(MYMVABINDIR)/TMVA%: $(MYMVABINDIR)/TMVA%.o $(MYMVABINDIR)/functions.o
	@echo -n "Linking $@ ... "
	$(CXX) $(CCFLAGS) $^ $(LDFLAGS) -L$(MYLIB) \
		-lMyTreeReader -lGenVector -lMLP -lMinuit -lTreePlayer -lTMVA -lTMVAGui -lXMLIO \
		-lRooFit -lRooFitCore -lMLP -lm  -g -o $@

$(MYMVABINDIR)/%.o: $(MYMVASRCDIR)/%.cc
	@echo -n "Building $@ ... "
	$(CXX) $(CCFLAGS) $<  $(addprefix -I, $(ROOTINCS)) -I$(OPENHF2020TOPDIR) -g -c -o $@
	@echo "Done"

$(MYMVABINDIR):
	@echo "Creating directory bin"
	@mkdir $(MYMVABINDIR)
	@echo "Done"
	@echo

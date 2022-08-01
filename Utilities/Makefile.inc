UtilitiesDir=$(OPENHF2020TOPDIR)/Utilities
TreeReaderDir=$(UtilitiesDir)/TreeReader
FitUtilsDir=$(UtilitiesDir)/FitUtils
Ana=$(UtilitiesDir)/Ana
MYLIB=$(UtilitiesDir)/lib

CXXFLAGS += -I$(shell HepMC3-config --includedir)
CXXFLAGS += $(shell HepMC3-config --libs)
LDFLAGS += $(shell HepMC3-config --ldflags)

$(MYLIB)/libMyTreeReader.so: \
	$(TreeReaderDir)/D0Event.cxx \
	$(TreeReaderDir)/ParticleTreeMC2.cxx \
	$(TreeReaderDir)/ParticleTreeMC.cxx \
	$(TreeReaderDir)/ParticleTreeData.cxx \
	$(TreeReaderDir)/ParticleTree.cxx \
	$(Ana)/Common.cc \
	$(Ana)/TreeHelpers.cc \
	$(Ana)/MyGenNTuple.cc \
	$(Ana)/MyNTuple.cc \
	$(MYLIB)/MyTreeReaderDict.cxx
	@echo "Generating shared library..."
	$(CXX) -shared -fPIC -o $@ $(CXXFLAGS) $(LDFLAGS) $^
	@echo "Done..."
	@echo

$(MYLIB)/MyTreeReaderDict.cxx: \
	$(TreeReaderDir)/D0Event.h \
	$(Ana)/MyGenNTuple.h \
	$(Ana)/MyNTuple.h \
	$(Ana)/Common.h \
	$(Ana)/TreeHelpers.h \
	$(TreeReaderDir)/ParticleTreeMC2.hxx \
	$(TreeReaderDir)/ParticleTreeMC.hxx \
	$(TreeReaderDir)/ParticleTreeData.hxx \
	$(TreeReaderDir)/ParticleTree.hxx \
	$(UtilitiesDir)/Linkdef.h
	@echo "Generating dictionary..."
	rootcling --inlineInputHeader -f $@ -rmf $(MYLIB)/MyTreeReaderDict.rootmap -rml $(MYLIB)/libMyTreeReader.so $^
	@echo "Done..."
	@echo

$(MYLIB)/libMyFitUtils.so: \
	$(MYLIB)/fitUtils.o \
	$(MYLIB)/fitUtilsD0.o \
	$(MYLIB)/ReadConfig.o \
	$(MYLIB)/MyFitUtilsDict.cxx
	@echo "Generating shared library for fit utilities.."
	$(CXX) -shared -fPIC -o $@ $(CXXFLAGS) -lRooFitCore -lRooFit $(LDGLIBFLAGS) $^
	rm $(MYLIB)/fitUtils.o $(MYLIB)/ReadConfig.o
	@echo "Done..."
	@echo

$(MYLIB)/%.o : $(FitUtilsDir)/%.cc
	$(CXX) -o $@ -fPIC -c $(CXXFLAGS) $<

$(MYLIB)/MyFitUtilsDict.cxx: \
	$(FitUtilsDir)/fitUtils.h $(FitUtilsDir)/ReadConfig.h \
	$(FitUtilsDir)/Linkdef.h
	@echo "Generating dictionary for fit utilities..."
	rootcling --inlineInputHeader -f $@ -rmf $(MYLIB)/MyFitUtilsDict.rootmap -rml $(MYLIB)/libMyFitUtils.so $^
	@echo "Done..."
	@echo

$(MYLIB):
	@echo "Creating directory lib"
	@mkdir $@
	@echo "done"
	@echo

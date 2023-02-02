UtilitiesDir=$(OPENHF2020TOPDIR)/Utilities
TreeReaderDir=$(UtilitiesDir)/TreeReader
FitUtilsDir=$(UtilitiesDir)/FitUtils
Ana=$(UtilitiesDir)/Ana
MYLIB=$(UtilitiesDir)/lib

CXXFLAGS += -I$(shell HepMC3-config --includedir)
HepMC3IncFlag=-I$(shell HepMC3-config --includedir)
# CXXFLAGS += $(shell HepMC3-config --libs)
LDFLAGS += $(shell HepMC3-config --ldflags)

# $(MYLIB)/libMyTreeReader.so: \
# 	$(TreeReaderDir)/D0Event.cxx \
# 	$(TreeReaderDir)/ParticleTreeMC2.cxx \
# 	$(TreeReaderDir)/ParticleTreeMC.cxx \
# 	$(TreeReaderDir)/ParticleTreeData.cxx \
# 	$(TreeReaderDir)/ParticleTree.cxx \
# 	$(Ana)/Common.cc \
# 	$(Ana)/TreeHelpers.cc \
# 	$(Ana)/MyGenNTuple.cc \
# 	$(Ana)/MyNTuple.cc \
# 	$(MYLIB)/MyTreeReaderDict.cxx
# 	@echo "Generating shared library..."
# 	$(CXX) -shared -fPIC -o $@ $(CXXFLAGS) $(LDFLAGS) $^
# 	@echo "Done..."
# 	@echo

$(MYLIB)/libMyTreeReader.so: \
	$(MYLIB)/D0Event.o \
	$(MYLIB)/ParticleTreeMC2.o \
	$(MYLIB)/ParticleTreeMC.o \
	$(MYLIB)/ParticleTreeData.o \
	$(MYLIB)/ParticleTree.o \
	$(MYLIB)/Common.o \
	$(MYLIB)/TreeHelpers.o \
	$(MYLIB)/MyGenNTuple.o \
	$(MYLIB)/MyNTuple.o \
	$(MYLIB)/MyTreeReaderDict.o
	@echo "Generating shared library..."
	$(CXX) -shared -fPIC -o $@ $(CXXFLAGS) $(LDFLAGS) $^
	@echo "Done..."
	@echo

$(MYLIB)/MyTreeReaderDict.o: $(MYLIB)/MyTreeReaderDict.cxx
	$(CXX) -fPIC -c -o $@ $(CXXFLAGS) $(LDFLAGS) $<

$(MYLIB)/%.o: $(Ana)/%.cc
	$(CXX) -fPIC -c -o $@ $(CXXFLAGS) $(LDFLAGS) $<

$(MYLIB)/%.o: $(TreeReaderDir)/%.cxx
	$(CXX) -fPIC -c -o $@ $(CXXFLAGS) $(LDFLAGS) $<

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
	rootcling --inlineInputHeader -f $@ -rmf $(MYLIB)/MyTreeReaderDict.rootmap -rml $(MYLIB)/libMyTreeReader.so $^ $(HepMC3IncFlag)
	@echo "Done..."
	@echo

# $(MYLIB)/libMyFitUtils.so: \
# 	$(MYLIB)/fitUtils.o \
# 	$(MYLIB)/fitUtilsD0.o \
# 	$(MYLIB)/ReadConfig.o \
# 	$(MYLIB)/MyFitUtilsDict.cxx
# 	@echo "Generating shared library for fit utilities.."
# 	$(CXX) -shared -fPIC -o $@ $(CXXFLAGS) -lRooFitCore -lRooFit $(LDGLIBFLAGS) $^
# 	rm $(MYLIB)/fitUtils.o $(MYLIB)/ReadConfig.o
# 	@echo "Done..."
# 	@echo

$(MYLIB)/libMyFitUtils.so: \
	$(MYLIB)/fitUtils.o \
	$(MYLIB)/fitUtilsD0.o \
	$(MYLIB)/ReadConfig.o \
	$(MYLIB)/MyFitUtilsDict.o
	@echo "Generating shared library for fit utilities.."
	$(CXX) -shared -fPIC -o $@ $(CXXFLAGS) -lRooFitCore -lRooFit $(LDGLIBFLAGS) $^
	@echo "Done..."
	@echo


$(MYLIB)/MyFitUtilsDict.o: $(MYLIB)/MyFitUtilsDict.cxx
	$(CXX) -o $@ -fPIC -c $(CXXFLAGS) $<

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

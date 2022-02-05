UtilitiesDir=$(OPENHF2020TOPDIR)/Utilities
TreeReaderDir=$(UtilitiesDir)/TreeReader
Ana=$(UtilitiesDir)/Ana
MYLIB=$(UtilitiesDir)/lib


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

$(MYLIB):
	@echo "Creating directory lib"
	@mkdir $@
	@echo "done"
	@echo

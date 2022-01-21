# Makefile
LIB=lib
TreeReaderDir=$(OPENHF2020TOP)/Utilities/TreeReader
Ana=$(OPENHF2020TOP)/Utilities/Ana

CXXFLAGS=$(shell root-config --cflags)
CXXFLAGS+=-I$(OPENHF2020TOP)
LDFLAGS=$(shell root-config --ldflags)
LDFLAGS+=$(shell root-config --libs)
CXX=$(shell root-config --cxx)

.PHONY: all
all: $(LIB) $(LIB)/libMyTreeReader.so

$(LIB)/libMyTreeReader.so: \
	$(TreeReaderDir)/D0Event.cxx \
	$(TreeReaderDir)/ParticleTreeMC2.cxx \
	$(TreeReaderDir)/ParticleTreeMC.cxx \
	$(TreeReaderDir)/ParticleTreeData.cxx \
	$(TreeReaderDir)/ParticleTree.cxx \
	$(Ana)/Common.cc \
	$(Ana)/TreeHelpers.cc \
	$(Ana)/MyGenNTuple.cc \
	$(Ana)/MyNTuple.cc \
	$(LIB)/MyTreeReaderDict.cxx
	@echo "Generating shared library..."
	$(CXX) -shared -fPIC -o $@ $(CXXFLAGS) $(LDFLAGS) $^
	@echo "Done..."
	@echo


$(LIB)/MyTreeReaderDict.cxx: \
	$(TreeReaderDir)/D0Event.h \
	$(Ana)/MyGenNTuple.h \
	$(Ana)/MyNTuple.h \
	$(Ana)/Common.h \
	$(Ana)/TreeHelpers.h \
	$(TreeReaderDir)/ParticleTreeMC2.hxx \
	$(TreeReaderDir)/ParticleTreeMC.hxx \
	$(TreeReaderDir)/ParticleTreeData.hxx \
	$(TreeReaderDir)/ParticleTree.hxx \
	Linkdef.h
	@echo "Generating dictionary..."
	rootcling -I$(OPENHF2020TOP) --inlineInputHeader -f $@ -rmf $(LIB)/MyTreeReaderDict.rootmap -rml $(LIB)/libMyTreeReader.so $^
	@echo "Done..."
	@echo

lib:
	@echo "Creating directory lib"
	@mkdir lib
	@echo "done"
	@echo

.PHONY: clean
clean:
	rm -r $(LIB)

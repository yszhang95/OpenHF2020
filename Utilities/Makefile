# Makefile
LIB=lib
TreeReaderDir=TreeReader
Ana=Ana

CXXFLAGS=$(shell root-config --cflags)
CXXFLAGS+=-I$(OPENHF2020TOP)/Utilities
LDFLAGS=$(shell root-config --ldflags)
LDFLAGS+=$(shell root-config --libs)
CXX=$(shell root-config --cxx)

.PHONY: all
all: $(LIB) $(LIB)/libMyTreeReader.so

$(LIB)/libMyTreeReader.so: \
	$(TreeReaderDir)/ParticleTreeMC2.cxx \
	$(TreeReaderDir)/ParticleTreeMC.cxx \
	$(TreeReaderDir)/ParticleTreeData.cxx \
	$(TreeReaderDir)/ParticleTree.cxx \
	$(Ana)/Common.cc \
	$(Ana)/TreeHelpers.cc \
	$(Ana)/MyNTuple.cc \
	$(LIB)/MyTreeReaderDict.cxx
	@echo "Generating shared library..."
	$(CXX) -shared -fPIC -o $@ $(CXXFLAGS) $(LDFLAGS) $^
	@echo "Done..."
	@echo


$(LIB)/MyTreeReaderDict.cxx: \
	$(Ana)/MyNTuple.h \
	$(Ana)/Common.h \
	$(Ana)/TreeHelpers.h \
	$(TreeReaderDir)/ParticleTreeMC2.hxx \
	$(TreeReaderDir)/ParticleTreeMC.hxx \
	$(TreeReaderDir)/ParticleTreeData.hxx \
	$(TreeReaderDir)/ParticleTree.hxx \
	Linkdef.h
	@echo "Generating dictionary..."
	rootcling -f $@ -rmf $(LIB)/MyTreeReaderDict.rootmap -rml $(LIB)/libMyTreeReader.so $^
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

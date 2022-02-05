.PHONY: all
all::
	@echo Start building...
	@echo

-include Utilities/Makefile.inc

all:: $(LIB) $(LIB)/libMyTreeReader.so

.PHONY: clean
clean:
	rm -rf $(LIB)

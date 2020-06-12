OBJDIR = build
#SRCDIR = man/src

TESTSRC = .
MAINSRC = main/src
TESTOBJ = $(OBJDIR)/test
MAINOBJ = $(OBJDIR)/main

# submodules
CCSPEC_HOME = ../submodules/ccspec

include ccspec.mk

CPLUS_INCLUDE_PATH = -I$(CCSPEC_HOME)/include
CXXFLAGS += -std=c++11
LDLIBS = $(OBJDIR)/libccspec.a
#MAIN = start

TESTOBJS := $(patsubst $(TESTSRC)/%.cc, $(TESTOBJ)/%.o, $(wildcard $(TESTSRC)/*.cc))
MAINOBJS := $(patsubst $(MAINSRC)/%.cc, $(MAINOBJ)/%.o, $(wildcard $(MAINSRC)/*.cc))

#init: $(TESTOBJ) $(MAINOBJ)

$(TESTOBJ) $(MAINOBJ):
	mkdir -p $@	

# run the tests
test: $(OBJDIR)/start
	$<

# link the tests
$(OBJDIR)/start: $(TESTOBJS) $(MAINOBJS) $(LDLIBS)
	$(CXX) -o $@ $^

# compile the tests
$(TESTOBJ)/%.o: $(TESTSRC)/%.cc $(TESTOBJ)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CPLUS_INCLUDE_PATH)
	
# compile the sources
$(MAINOBJ)/%.o: $(MAINSRC)/%.cc $(MAINOBJ)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CPLUS_INCLUDE_PATH)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/*
	
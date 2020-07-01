.SUFFIXES:

DEBUG := -g -O0

OBJDIR = build
#SRCDIR = man/src

TESTSRC = spec
MAINSRC = src
TESTOBJ = $(OBJDIR)/test
MAINOBJ = $(OBJDIR)/main

# submodules
CCSPEC_HOME = submodules/ccspec

include ccspec.mk

CPLUS_INCLUDE_PATH = -I$(CCSPEC_HOME)/include
CXXFLAGS += -std=c++11  $(DEBUG)
LDLIBS = $(OBJDIR)/libccspec.a
#MAIN = start

TESTOBJS := $(patsubst $(TESTSRC)/%.cc, $(TESTOBJ)/%.o, $(wildcard $(TESTSRC)/*.cc $(TESTSRC)/helpers/*.cc))
MAINOBJS := $(patsubst $(MAINSRC)/%.c, $(MAINOBJ)/%.o, $(wildcard $(MAINSRC)/*.c))

init: $(TESTOBJ) $(MAINOBJ) $(TESTOBJ)/helpers

$(TESTOBJ) $(MAINOBJ) $(TESTOBJ)/helpers:
	mkdir -p $@	

# run the tests
test: $(OBJDIR)/start
	$<

# link the tests
$(OBJDIR)/start: $(TESTOBJS) $(MAINOBJS) $(LDLIBS)
	$(CXX) -o $@ $^

# some explicit .h dependencies
$(TESTOBJS): $(MAINSRC)/gd.h $(MAINSRC)/gd_internal.h

# compile the tests
$(TESTOBJ)/%.o: $(TESTSRC)/%.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CPLUS_INCLUDE_PATH) -I$(MAINSRC)
	
# compile the sources
$(MAINOBJ)/%.o: $(MAINSRC)/%.c
	$(CXX) -c -o $@ $^ $(CXXFLAGS) $(CPLUS_INCLUDE_PATH)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/*
	
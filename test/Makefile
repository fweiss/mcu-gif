.SUFFIXES:
.SUFFIXES: .c .o

SRCDIR = ../src
TESTDIR = .
OBJDIR = build

GTEST_FLAGS = --gtest_filter=DecodeLzw.*

include local.mk
include gtest.mk

CPLUS_INCLUDE_PATH = -I$(SRCDIR) -I$(GTEST_HOME)/googletest/include -I$(FFF_HOME)
C_INCLUDE_PATH = -I$(SRCDIR) -I$(GTEST_HOME)/googletest/include -I$(FFF_HOME)
CXXFLAGS += -std=c++11 -stdlib=libc++ -DGTEST_USE_OWN_TR1_TUPLE=1
LDLIBS = $(OBJDIR)/libgtest.a $(OBJDIR)/libgmock.a

UNIT = gd
TEST = $(UNIT)-test

all: $(OBJDIR)/$(TEST).o $(LDLIBS) $(OBJDIR)/gtest_main.o $(OBJDIR)/$(UNIT).o

debug:
	@echo $(SRCDIR) $(TESTDIR) $(OBJDIR)

# run the test
test: $(OBJDIR)/$(TEST)
	$<  $(GTEST_FLAGS)

# link the test
$(OBJDIR)/$(TEST): $(OBJDIR)/$(TEST).o $(LDLIBS) $(OBJDIR)/gtest_main.o $(OBJDIR)/$(UNIT).o $(OBJDIR)/$(UNIT).o
	$(CXX) -o $@ $^

# compile the tests
$(OBJDIR)/%.o: $(TESTDIR)/%.cpp $(SRCDIR)/$(UNIT).h
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(CPLUS_INCLUDE_PATH) -o $@ $<

# compile the source
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/$(UNIT).h
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $(C_INCLUDE_PATH) -I. -o $@ $<

.PHONY: clean
clean:
	rm $(OBJDIR)/*

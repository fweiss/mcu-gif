.SUFFIXES:
.SUFFIXES: .c .o

SRCDIR = ../src
TESTDIR = .
OBJDIR = build

# can use wildcards, default is all, others class.*, etc
# currently these are not working:
GTEST_FLAGS = --gtest_filter=-RenderSquaresImage.ipixels:DecodeSubBlock.simple

# submodules
GTEST_HOME = ../remote/googletest
FFF_HOME = ../remote/fff

include gtest.mk

#DEBUG := -g -O0

CPLUS_INCLUDE_PATH = -I$(SRCDIR) -I$(GTEST_HOME)/googletest/include -I$(FFF_HOME)
C_INCLUDE_PATH = -I$(SRCDIR) -I$(GTEST_HOME)/googletest/include -I$(FFF_HOME)
CXXFLAGS += -std=c++11 -DGTEST_USE_OWN_TR1_TUPLE=1 $(DEBUG)
LDLIBS = $(OBJDIR)/libgtest.a $(OBJDIR)/libgmock.a

UNIT = gd
TEST = $(UNIT)-test

# all the test .o files that need to be linked
TEST_OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *-test.cpp))

all: $(OBJDIR)/$(TEST).o $(LDLIBS) $(OBJDIR)/gtest_main.o $(OBJDIR)/$(UNIT).o

debug:
	@echo $(SRCDIR) $(TESTDIR) $(OBJDIR)

# run the test
test: $(OBJDIR)/$(TEST)
	$<  $(GTEST_FLAGS)

# link the test
# $(OBJDIR)/gtest_main.o 
$(OBJDIR)/$(TEST): $(OBJDIR)/$(UNIT).o $(OBJDIR)/$(UNIT).o $(OBJDIR)/gtest_main.o $(TEST_OBJS) $(LDLIBS)
	$(CXX) -o $@ $^ -lpthread

# compile the tests
$(OBJDIR)/%.o: $(TESTDIR)/%.cpp $(SRCDIR)/$(UNIT).h
	$(CXX) -c -o $@ $< $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(CPLUS_INCLUDE_PATH)

# compile the source
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/$(UNIT).h
	$(CXX) -c -o $@ $< $(CFLAGS) $(CPPFLAGS) $(C_INCLUDE_PATH) -I. $(DEBUG)

# but don't clean hiddn files like .gitignore
.PHONY: clean
clean:
	rm $(OBJDIR)/*

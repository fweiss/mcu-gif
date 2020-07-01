# create the ccspec library

VPATH = \
	$(CCSPEC_HOME)/src/core: \
	$(CCSPEC_HOME)/src/core/formatters: \
	$(CCSPEC_HOME)/src/matchers: \
	$(CCSPEC_HOME)/src/support: \

# there are ways to do recursive directories, just taking the easy way out here
# also flattening is not foolproof, but good enought for now
CCSPEC_SRCS = $(wildcard $(CCSPEC_HOME)/src/core/formatters/*.cc) $(wildcard $(CCSPEC_HOME)/src/*/*.cc)
CCSPEC_OBJS = $(patsubst %.cc,$(OBJDIR)/%.o,$(CCSPEC_SRCS))
CCSPEC_O = $(patsubst build/submodules/ccspec/src/%,build/ccspec/%,$(CCSPEC_OBJS))
CCSPEC_F = $(patsubst %.cc,$(OBJDIR)/%.o,$(notdir $(CCSPEC_SRCS)))

# compile ccspec
$(OBJDIR)/%.o: %.cc
	$(CXX) -c -o $@ $< $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -I$(CCSPEC_HOME)/include

# make ccpec librRY
$(OBJDIR)/libccspec.a: $(CCSPEC_F)
	$(AR) -r $@ $^
	
.PHONY: dira
dira:
	mkdir -p build/ccspec build/ccspec/core build/ccspec/core/formatters build/ccspec/matchers build/ccspec/support
	mkdir -p build/submodules/ccspec/src build/submodules/ccspec/src/core build/submodules/ccspec/src/core/formatters build/submodules/ccspec/src/matchers build/submodules/ccspec/src/support
	
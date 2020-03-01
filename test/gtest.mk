# create the gtest and gmock libraries
$(OBJDIR)/gtest-all.o: $(GTEST_HOME)/googletest/src/gtest-all.cc
	$(CXX) -c -o $@ $< $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -I$(GTEST_HOME)/googletest/include -I$(GTEST_HOME)/googletest

$(OBJDIR)/gtest_main.o: $(GTEST_HOME)/googletest/src/gtest_main.cc
	$(CXX) -c -o $@ $< $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -I$(GTEST_HOME)/googletest/include

$(OBJDIR)/libgtest.a: $(OBJDIR)/gtest-all.o
	$(AR) -rv $@ $<

$(OBJDIR)/gmock-all.o: $(GTEST_HOME)/googlemock/src/gmock-all.cc
	$(CXX) -c -o $@ $< $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -I$(GTEST_HOME)/googlemock/include -I$(GTEST_HOME)/googlemock -I$(GTEST_HOME)/googletest/include

$(OBJDIR)/libgmock.a: $(OBJDIR)/gmock-all.o
	$(AR) -rv $@ $<

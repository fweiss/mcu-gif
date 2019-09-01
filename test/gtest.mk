# create the gtest and gmock libraries
$(OBJDIR)/gtest-all.o:
	$(CXX) -c -o $@ $< $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -I$(GTEST_HOME)/googletest/include -I$(GTEST_HOME)/googletest -c $(GTEST_HOME)/googletest/src/gtest-all.cc

$(OBJDIR)/gtest_main.o:
	$(CXX) -c -o $@ $< $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -I$(GTEST_HOME)/googletest/include -c $(GTEST_HOME)/googletest/src/gtest_main.cc

$(OBJDIR)/libgtest.a: $(OBJDIR)/gtest-all.o
	ar -rv $@ $<

$(OBJDIR)/gmock-all.o: $(GTEST_HOME)/googlemock/src/gmock-all.cc
	$(CXX) -c -o $@ $< $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -I$(GTEST_HOME)/googlemock/include -I$(GTEST_HOME)/googlemock -I$(GTEST_HOME)/googletest/include

$(OBJDIR)/libgmock.a: $(OBJDIR)/gmock-all.o
	ar -rv $@ $<

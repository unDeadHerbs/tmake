.INTERMEDIATE: tmake.o
tmake.o .d/tmake.d : tmake.cpp
tmake.bin: tmake.o
	$(CXX) $(CXXFLAGS) $(LIBARYFLAGS) -o $@ $^

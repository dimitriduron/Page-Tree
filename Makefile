# CXX Make variable for compiler
CXX=g++
# Make variable for compiler options
#	-std=c++11  C/C++ variant to use, e.g. C++ 2011
#	-g          include information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -g

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
pagingwithtlb : driver.o tracereader.o output_mode_helpers.o
	$(CXX) $(CXXFLAGS) -o pagingwithtlb $^


driver.o:				driver.cpp
tracereader.o:			tracereader.h tracereader.c
output_mode_helpers.o:	output_mode_helpers.h output_mode_helpers.c

clean :
	rm *.o


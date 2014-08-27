CXX = g++
CXXFLAGS = -c -ggdb -std=c++11

all: test libargs.a

test: test.cpp libargs.a
	$(CXX) $< -ggdb -L. -largs -o $@

libargs.a: args.o
	ar rcs -o $@ $<

args.o: args.cpp args.h
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	rm args.o && rm libargs.a


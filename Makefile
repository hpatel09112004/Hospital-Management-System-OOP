CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Iinclude -MMD -MP

LIB_SRC := $(filter-out src/main.cpp,$(wildcard src/*.cpp))
LIB_OBJ := $(LIB_SRC:src/%.cpp=build/%.o)

.PHONY: all run demo test clean

all: hms

hms: $(LIB_OBJ) build/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build/tests: tests/test_hospital.cpp $(LIB_OBJ) | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

build:
	mkdir -p build

run: hms
	./hms

demo: hms
	./hms --demo

test: build/tests
	./build/tests

clean:
	rm -rf build hms hms.exe demo_data

-include $(wildcard build/*.d)

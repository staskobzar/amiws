
.PHONY: all build install clean test doc package cov

all: build

build:
	@test -d build || mkdir build
	@cd build && cmake .. && make

test:
	@cd build && ctest

install: build
	@cd build && make install

cov:
	@test -d build || mkdir build
	@cd build && cmake .. -DCMAKE_C_FLAGS='-g -O0 -Wall -W -Wshadow -Wunused-variable -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage'
	@cd build && make && make test
	@cd build && ctest -T coverage

doc:
	@cd build && make doc

package: build
	@cd build && make package

clean:
	@rm -rf build

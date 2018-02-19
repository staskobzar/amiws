
.PHONY: all build install clean test doc package

all: build

build:
	@test -d build || mkdir build
	@cd build && cmake .. && make

test:
	@cd build && ctest

install: build
	@cd build && make install

doc:
	@cd build && make doc

package: build
	@cd build && make package

clean:
	@rm -rf build

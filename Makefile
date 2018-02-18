
.PHONY: all build install clean test

all: build

build:
	@test -d build || mkdir build
	@cd build && cmake .. && make

test:
	@cd build && ctest

install: build

clean:
	@rm -rf build

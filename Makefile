
.PHONY: all build install clean test doc

all: build

build:
	@test -d build || mkdir build
	@cd build && cmake .. && make

test:
	@cd build && ctest

install: build

doc:
	@cd build && make doc

clean:
	@rm -rf build

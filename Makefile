
.PHONY: all build install clean

all: build

build:
	@test -d build || mkdir build
	@cd build && cmake .. && make

install: build

clean:
	@rm -rf build

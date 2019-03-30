
.PHONY: all
all: compile test

build:
	mkdir -p build

.PHONY: cmake
cmake: build
	cd build && cmake ..

.PHONY: compile
compile: cmake
	make -s -C build

.PHONY: test
test: compile
	make -s -C build test ARGS=-V

.PHONY: clean
clean:
	make -s -C build clean

.PHONY: cleanall
cleanall:
	rm -rf build

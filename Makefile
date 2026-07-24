CMAKE := cmake
CXX := /usr/bin/clang++
BUILD_DIR := build
BUILD_TYPE ?= Debug

.PHONY: all configure build app example clean

all: build

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_CXX_COMPILER=$(CXX)

build: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel

app: configure
	$(CMAKE) --build $(BUILD_DIR) --target raytracer --parallel
	$(BUILD_DIR)/bin/raytracer

example: configure
	$(CMAKE) --build $(BUILD_DIR) --target sdl_example --parallel
	$(BUILD_DIR)/bin/sdl_example

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)

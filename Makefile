CMAKE := cmake
CXX := /usr/bin/clang++
BUILD_DIR := build
BUILD_TYPE ?= Debug

KNOWN_GOALS := all configure build debug release example clean
RAW_ARGS := $(filter-out $(KNOWN_GOALS),$(MAKECMDGOALS))
POSITIONAL_ARGS := $(filter-out -r,$(RAW_ARGS))
HAS_RECURSION_FLAG := $(or \
	$(filter -r,$(RAW_ARGS)), \
	$(findstring r,$(firstword $(MAKEFLAGS))))

ifneq ($(HAS_RECURSION_FLAG),)
ifneq ($(word 2,$(POSITIONAL_ARGS)),)
SCENE_GOAL := $(firstword $(POSITIONAL_ARGS))
RECURSION_GOAL := $(word 2,$(POSITIONAL_ARGS))
else
RECURSION_GOAL := $(firstword $(POSITIONAL_ARGS))
endif
else
SCENE_GOAL := $(firstword $(POSITIONAL_ARGS))
endif

SCENE ?= $(if $(SCENE_GOAL),$(SCENE_GOAL),ballcube)
RECURSION ?= $(if $(RECURSION_GOAL),$(RECURSION_GOAL),1)

ifneq ($(RAW_ARGS),)
.PHONY: $(RAW_ARGS)
$(RAW_ARGS):
	@:
endif

.PHONY: all configure build debug release example clean

all: build

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_CXX_COMPILER=$(CXX)

build: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel

debug: configure
	$(CMAKE) --build $(BUILD_DIR) --target raytracer --parallel
	$(BUILD_DIR)/bin/raytracer $(SCENE) -r $(RECURSION)

release:
	$(MAKE) debug BUILD_TYPE=Release BUILD_DIR=build-release \
		SCENE=$(SCENE) RECURSION=$(RECURSION)

example: configure
	$(CMAKE) --build $(BUILD_DIR) --target sdl_example --parallel
	$(BUILD_DIR)/bin/sdl_example

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)

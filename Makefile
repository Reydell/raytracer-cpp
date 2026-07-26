CMAKE := cmake
CXX := /usr/bin/clang++
BUILD_DIR := build
BUILD_TYPE ?= Debug

KNOWN_GOALS := all configure build debug release example clean
RAW_ARGS := $(filter-out $(KNOWN_GOALS),$(MAKECMDGOALS))
argument_value = $(strip \
	$(if $(filter $(1),$(firstword $(2))), \
		$(word 2,$(2)), \
		$(if $(word 2,$(2)), \
			$(call argument_value,$(1),$(wordlist 2,$(words $(2)),$(2))) \
		) \
	) \
)

HAS_EXPLICIT_RECURSION_FLAG := $(filter -r,$(RAW_ARGS))
HAS_RECURSION_FLAG := $(or \
	$(filter -r,$(RAW_ARGS)), \
	$(findstring r,$(firstword $(MAKEFLAGS))))

ifneq ($(HAS_EXPLICIT_RECURSION_FLAG),)
ifneq ($(filter-out -%,$(firstword $(RAW_ARGS))),)
SCENE_GOAL := $(firstword $(RAW_ARGS))
endif
RECURSION_GOAL := $(call argument_value,-r,$(RAW_ARGS))
else ifneq ($(HAS_RECURSION_FLAG),)
ifneq ($(word 2,$(RAW_ARGS)),)
SCENE_GOAL := $(firstword $(RAW_ARGS))
RECURSION_GOAL := $(word 2,$(RAW_ARGS))
else
RECURSION_GOAL := $(firstword $(RAW_ARGS))
endif
else
ifneq ($(filter-out -%,$(firstword $(RAW_ARGS))),)
SCENE_GOAL := $(firstword $(RAW_ARGS))
endif
endif

WIDTH_GOAL := $(call argument_value,-w,$(RAW_ARGS))
HEIGHT_GOAL := $(call argument_value,-h,$(RAW_ARGS))

SCENE ?= $(if $(SCENE_GOAL),$(SCENE_GOAL),ballcube)
RECURSION ?= $(if $(RECURSION_GOAL),$(RECURSION_GOAL),1)
WIDTH ?= $(if $(WIDTH_GOAL),$(WIDTH_GOAL),1200)
HEIGHT ?= $(if $(HEIGHT_GOAL),$(HEIGHT_GOAL),800)
RAYTRACER_ARGS = $(SCENE) -r $(RECURSION) -w $(WIDTH) -h $(HEIGHT)

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
	$(BUILD_DIR)/bin/raytracer $(RAYTRACER_ARGS)

release:
	$(MAKE) debug BUILD_TYPE=Release BUILD_DIR=build-release \
		SCENE=$(SCENE) RECURSION=$(RECURSION) \
		WIDTH=$(WIDTH) HEIGHT=$(HEIGHT)

example: configure
	$(CMAKE) --build $(BUILD_DIR) --target sdl_example --parallel
	$(BUILD_DIR)/bin/sdl_example

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)

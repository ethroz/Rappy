# Quiet makefile.
MAKEFLAGS += --silent

# Get all the binaries.
BINSRC := $(wildcard *.cpp)
BINS := $(BINSRC:%.cpp=%)

# Make all rule.
.PHONY: all clean debug normal release $(BINS)
.ONESHELL:
all:
	@echo Building all
	@date +%s > /tmp/.time
	@cd build
	$(MAKE)
	@echo "Done ($$(($$(date +%s)-$$(cat /tmp/.time)))s)"

# Clean rule.
.ONESHELL:
clean:
	@rm -rf build
	@mkdir -p build
	@cd build
	@cmake ..

# Debug rule
.ONESHELL:
debug:
	@echo Building all for Debug
	@date +%s > /tmp/.time
	@mkdir -p build
	@cd build
	@cmake .. -DCMAKE_BUILD_TYPE=Debug
	$(MAKE)
	@echo "Done ($$(($$(date +%s)-$$(cat /tmp/.time)))s)"

# Debug rule
.ONESHELL:
normal:
	@echo Building all unoptimized
	@date +%s > /tmp/.time
	@mkdir -p build
	@cd build
	@cmake .. -DCMAKE_BUILD_TYPE=
	$(MAKE)
	@echo "Done ($$(($$(date +%s)-$$(cat /tmp/.time)))s)"

# Release rule
.ONESHELL:
release:
	@echo Building all for Release
	@date +%s > /tmp/.time
	@mkdir -p build
	@cd build
	@cmake .. -DCMAKE_BUILD_TYPE=Release
	$(MAKE)
	@echo "Done ($$(($$(date +%s)-$$(cat /tmp/.time)))s)"

# Pattern to build a specific binary.
define BINARY_RULE
.ONESHELL:
$(1):
	@echo Building $(1)
	@date +%s > /tmp/.time
	@cd build
	$(MAKE) $(1)
	@echo "Done ($$$$(($$$$(date +%s)-$$$$(cat /tmp/.time)))s)"
endef

$(foreach bin,$(BINS),$(eval $(call BINARY_RULE,$(bin))))

include Makefile.common

BINS := $(BINSRC:%.cpp=%)

# Make sure the directories exist.
$(shell mkdir -p $(DEPDIR) $(OBJDIR) $(BINDIR))

# Quiet makefile.
MAKEFLAGS += --silent

# Make all rule.
.PHONY: all clean none $(BINS)
all:
	@echo Building all
	@date +%s > /tmp/.time
	$(MAKE) -f Makefile.d
	$(MAKE) -f Makefile.mk
	@echo "Done ($$(($$(date +%s)-$$(cat /tmp/.time)))s)"

# Clean rule.
clean:
	rm -rf $(OBJDIR)/* $(DEPDIR)/* $(DEPDIR)/.stamp* $(BINDIR)/*

# Pattern to build a specific binary.
define BINARY_RULE
$(1):
	@echo Building $(1)
	$(MAKE) -f Makefile.d
	$(MAKE) -f Makefile.mk bin/$(1)
endef

$(foreach bin,$(BINS),$(eval $(call BINARY_RULE,$(bin))))

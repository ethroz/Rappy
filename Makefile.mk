include Makefile.common

# Make all rule.
.PHONY: all
all: $(BIN)

# Build bin rules.
include $(BINDEP)

# Build obj rules.
include $(DEP)


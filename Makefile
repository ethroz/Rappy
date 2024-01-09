include Makefile.common

# Make sure the directories exist.
$(shell mkdir -p $(DEPDIR) $(OBJDIR) $(BINDIR))

# Quiet makefile.
MAKEFLAGS += --silent

# Multithreaded (but not all the threads)
MAKEFLAGS += -j3

# Make all rule.
.PHONY: all clean
all:
	@date +%s > /tmp/.time
	$(MAKE) -f Makefile.d
	$(MAKE) -f Makefile.mk
	@echo "Done ($$(($$(date +%s)-$$(cat /tmp/.time)))s)"

# Clean rule.
clean:
	rm -rf $(OBJDIR)/* $(DEPDIR)/* $(DEPDIR)/.stamp* $(BINDIR)/*


include Makefile.common

# Define the libraries to link with.
LIB := -lpthread -lpigpio -lrt -L./lib/bin -lfmt

# Define the includes.
INCDIR := .
INC    := -I $(INCDIR) -I ./lib/include

# Filter the sources to get the common obj files.
COMMON := $(filter-out $(addprefix ./,$(BINSRC)), $(SRC))
OBJS := $(COMMON:.%.cpp=$(OBJDIR)%$(D).o)

# Create stamps to force rebuilds.
STAMP    := $(DEPDIR)/.stamp
BINSTAMP := $(DEPDIR)/.stampbin
MKF := $(wildcard Makefile*)

# Create a stamp to force debug/release binary rebuild.
ifneq ($(file <$(BINSTAMP)), $(D))
	_ := $(file >$(BINSTAMP),$(D))
else ifeq (,$(wildcard $(BINSTAMP)))
	_ := $(file >$(BINSTAMP),$(D))
endif

# Build all rule.
.PHONY: all
all: $(BINDEP) $(DEP)

# Build bin dependency rule.
.ONESHELL:
$(DEPDIR)/%_bin$(D).d: $(DEPDIR)/%$(D).d
	# Steal the dependencies from the corresponding cpp file for this binary.
	# Replace all the .h with .c
	# Filter the remaining files based on if they exist.
	# Convert the cpp files into their respective object files in the obj directory.
	# Lastly, create a makefile rule from the dependencies.
	#@sed 's/\(\t.*\|.*: \)//' $< | paste -s -d "" | sed 's/\.h/\.c/g' | xargs ls -1 2>/dev/null | paste -s -d " " | sed -E 's#([^ ]+)\.cpp#$(OBJDIR)/\1$(D).o#g' | sed 's#\(.*\)#$(<:$(DEPDIR)/%$(D).d=$(BINDIR)/%): \1 $(BINSTAMP)#' > $@
	#
	@echo "$(<:$(DEPDIR)/%$(D).d=$(BINDIR)/%): $(<:$(DEPDIR)/%.d=$(OBJDIR)/%.o) $(OBJS) $(BINSTAMP)" > $@	
	@echo "\t@echo $$'(notdir $$'@)" | sed "s/'//g" >> $@
	@echo "\tsudo $(CC) $(CFLAGS) $$'(filter-out $(BINSTAMP), $$'^) $(LIB) -o $$'@" | sed "s/'//g" >> $@
	@echo "\tsudo chmod u+s $$'@" | sed "s/'//g" >> $@

# Build cpp file dependency rule.
.ONESHELL:
$(DEPDIR)/%$(D).d: %.cpp $(STAMP)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC) -MM -MT '$(<:%.cpp=$(OBJDIR)/%$(D).o)' $< | sed 's/ \\//' | paste -s -d "" | sed 's#\(.*\)#\1 $(STAMP)#' > $@
	@echo "\t@mkdir -p $$'(dir $$'@)" | sed "s/'//g" >> $@
	@echo "\t@echo $$'(notdir $$'<)" | sed "s/'//g" >> $@
	@echo "\t$(CC) $(CFLAGS) $(INC) -o $$'@ -c $$'<" | sed "s/'//g" >> $@

# Build stamp rule.
$(STAMP): $(MKF)
	touch $(STAMP)

